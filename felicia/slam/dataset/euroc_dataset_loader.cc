#include "felicia/slam/dataset/euroc_dataset_loader.h"

#include "third_party/chromium/base/strings/utf_string_conversions.h"

#include "felicia/core/lib/file/yaml_reader.h"
#include "felicia/slam/dataset/dataset_utils.h"

namespace felicia {
namespace slam {

EurocDatasetLoader::EurocDatasetLoader(const base::FilePath& path,
                                       DataKind data_kind)
    : path_(path), data_kind_(data_kind), current_(0) {
  path_to_data_list_ = PathToDataList();
  path_to_data_ = PathToData();
}

StatusOr<SensorMetaData> EurocDatasetLoader::Init() {
  YamlReader reader;
  Status s = reader.Open(PathToMetaData());
  if (!s.ok()) return s;
  SensorMetaData sensor_meta_data;
  switch (data_kind_) {
    case CAM0:
    case CAM1: {
      float frame_rate = reader["rate_hz"].as<float>();
      YAML::Node resolution = reader["resolution"];
      YAML::Node intrinsics = reader["intrinsics"];
      YAML::Node distortion_coeffs = reader["distortion_coefficients"];
      sensor_meta_data.set_color_camera_format(drivers::CameraFormat{
          resolution[0].as<int>(), resolution[1].as<int>(), PIXEL_FORMAT_BGR,
          frame_rate});
      sensor_meta_data.set_left_K(EigenCameraMatrixd{
          intrinsics[0].as<double>(), intrinsics[1].as<double>(),
          intrinsics[2].as<double>(), intrinsics[3].as<double>()});
      sensor_meta_data.set_left_D(EigenDistortionMatrixd{
          distortion_coeffs[0].as<double>(), distortion_coeffs[1].as<double>(),
          distortion_coeffs[2].as<double>(),
          distortion_coeffs[3].as<double>()});
      break;
    }
    // TODO: Fill the belows.
    case IMU0:
    case LEICA0:
    case GROUND_TRUTH:
      return errors::Unimplemented("");
      break;
  }
  return sensor_meta_data;
}

StatusOr<SensorData> EurocDatasetLoader::Next() {
  if (!reader_.IsOpened()) {
    Status s = reader_.Open(path_to_data_list_, ",", 1);
    if (!s.ok()) return s;
  }

  ++current_;
  int current_line = current_ + 1;
  std::vector<std::string> items;
  SensorData sensor_data;
  if (reader_.ReadItems(&items)) {
    if (items.size() != static_cast<size_t>(ColumnsForData())) {
      return errors::InvalidArgument(base::StringPrintf(
          "The number of columns is not valid at %" PRFilePath ":%d",
          path_to_data_list_.value().c_str(), current_line));
    }
    StatusOr<double> status_or =
        TryConvertToDouble(items[0], path_to_data_list_, current_line);
    if (!status_or.ok()) return status_or.status();
    sensor_data.set_timestamp(status_or.ValueOrDie());

    switch (data_kind_) {
      case CAM0: {
#if defined(OS_WIN)
        sensor_data.set_left_image_filename(
            base::UTF16ToUTF8(path_to_data_.AppendASCII(items[1]).value()));
#else
        sensor_data.set_left_image_filename(
            path_to_data_.AppendASCII(items[1]).value());
#endif
        break;
      }
      case CAM1: {
#if defined(OS_WIN)
        sensor_data.set_right_image_filename(
            base::UTF16ToUTF8(path_to_data_.AppendASCII(items[1]).value()));
#else
        sensor_data.set_right_image_filename(
            path_to_data_.AppendASCII(items[1]).value());
#endif
        break;
      }
      case IMU0: {
        float v[6];
        for (int i = 0; i < 6; ++i) {
          status_or =
              TryConvertToDouble(items[i + 1], path_to_data_, current_line);
          if (!status_or.ok()) return status_or.status();
          v[i] = status_or.ValueOrDie();
        }
        drivers::ImuFrame imu_frame;
        imu_frame.set_linear_acceleration(Vector3f{v[0], v[1], v[2]});
        imu_frame.set_angular_velocity(Vector3f{v[0], v[1], v[2]});
        sensor_data.set_imu_frame(imu_frame);
        break;
      }
      case LEICA0: {
        float v[3];
        for (int i = 0; i < 3; ++i) {
          status_or =
              TryConvertToDouble(items[i + 1], path_to_data_, current_line);
          if (!status_or.ok()) return status_or.status();
          v[i] = status_or.ValueOrDie();
        }
        sensor_data.set_point(Point3f{v[0], v[1], v[2]});
        break;
      }
      case GROUND_TRUTH: {
        float v[16];
        for (int i = 0; i < 16; ++i) {
          status_or =
              TryConvertToDouble(items[i + 1], path_to_data_, current_line);
          if (!status_or.ok()) return status_or.status();
          v[i] = status_or.ValueOrDie();
        }
        Point3f p(v[0], v[1], v[2]);
        Quaternionf q(v[4], v[5], v[6], v[3]);
        sensor_data.set_pose(Pose3f{p, q});
        sensor_data.set_velocity(Vector3f{v[7], v[8], v[9]});
        sensor_data.set_angular_velocity(Vector3f{v[10], v[11], v[12]});
        sensor_data.set_acceleration(Vector3f{v[13], v[14], v[15]});
        break;
      }
    }
  }
  return sensor_data;
}

bool EurocDatasetLoader::End() const { return reader_.eof(); }

base::FilePath EurocDatasetLoader::PathToMetaData() const {
  return PathToDataKind().AppendASCII("sensor.yaml");
}

base::FilePath EurocDatasetLoader::PathToData() const {
  return PathToDataKind().AppendASCII("data");
}

base::FilePath EurocDatasetLoader::PathToDataList() const {
  return PathToDataKind().AppendASCII("data.csv");
}

base::FilePath EurocDatasetLoader::PathToDataKind() const {
  switch (data_kind_) {
    case CAM0:
      return path_.AppendASCII("cam0");
    case CAM1:
      return path_.AppendASCII("cam1");
    case IMU0:
      return path_.AppendASCII("cam1");
    case LEICA0:
      return path_.AppendASCII("leica0");
    case GROUND_TRUTH:
      return path_.AppendASCII("state_groundtruth_estimate0");
  }
  NOTREACHED();
  return path_;
}

int EurocDatasetLoader::ColumnsForData() const {
  switch (data_kind_) {
    case CAM0:
      return 2;
    case CAM1:
      return 2;
    case IMU0:
      return 7;
    case LEICA0:
      return 4;
    case GROUND_TRUTH:
      return 17;
  }
  NOTREACHED();
  return 0;
}

}  // namespace slam
}  // namespace felicia