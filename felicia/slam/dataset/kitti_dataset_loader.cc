#include "felicia/slam/dataset/kitti_dataset_loader.h"

#include <iomanip>
#include <sstream>

#include "third_party/chromium/base/bits.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/file/csv_reader.h"
#include "felicia/slam/dataset/dataset_utils.h"

namespace felicia {
namespace slam {

KittiDatasetLoader::State::State(SensorData::DataType data_type,
                                 const base::FilePath& path)
    : data_type_(data_type),
      path_(path),
      path_to_data_(PathToData()),
      path_to_times_(path_.AppendASCII("times.txt")) {}

SensorData::DataType KittiDatasetLoader::State::data_type() const {
  return data_type_;
}

StatusOr<SensorMetaData> KittiDatasetLoader::State::Init() {
  CsvReader reader;
  enum { P0, P1, P2, P3, TR };
  int pos = SkipHeadCountForCalibs();
  int current_line = pos + 1;
  base::FilePath calibs_path = path_.AppendASCII("calibs.txt");
  Status s = reader.Open(calibs_path, " ", pos);
  if (!s.ok()) return s;
  std::vector<std::string> rows;
  if (!reader.ReadRows(&rows))
    return errors::InvalidArgument(
        base::StringPrintf("Failed to read the row: %d.", current_line));

  SensorMetaData sensor_meta_data;
  if (pos <= P3 && rows.size() != 13) {
    return errors::InvalidArgument(base::StringPrintf(
        "Invalid projection matrix at line %" PRFilePath ":%d",
        calibs_path.value().c_str(), current_line));
  }

  if ((pos == P0 && rows[0] == "P0:") || (pos == P1 && rows[0] == "P1:") ||
      (pos == P2 && rows[0] == "P2:") || (pos == P3 && rows[0] == "P3:")) {
    Eigen::Matrix<double, 3, 4> m;
    for (int i = 0; i < 12; ++i) {
      StatusOr<double> status_or =
          TryConvertToDouble(rows[i + 1], calibs_path, current_line);
      if (!status_or.ok()) return status_or.status();
      m(i / 4, i % 4) = status_or.ValueOrDie();
    }

    if (pos == P0 || pos == P2) {
      sensor_meta_data.set_left_P(EigenProjectionMatrixd{m});
    } else if (pos == P1 || pos == P3) {
      sensor_meta_data.set_right_P(EigenProjectionMatrixd{m});
    }
  } else {
    return errors::InvalidArgument(base::StringPrintf(
        "The first column isn't valid :%s.", rows[0].c_str()));
  }

  return sensor_meta_data;
}

StatusOr<SensorData> KittiDatasetLoader::State::Next() {
  SensorData sensor_data;
  size_t current = current_++;
  std::string line;
  base::TimeDelta timestamp;

  if (should_read_times_) {
    if (!times_reader_.IsOpened()) {
      times_reader_.set_option(BufferedReader::REMOVE_CR_OR_LF);
      Status s = times_reader_.Open(path_to_times_);
      if (!s.ok()) {
        if (data_type_ == SensorData::DATA_TYPE_LEFT_CAMERA ||
            data_type_ == SensorData::DATA_TYPE_RIGHT_CAMERA ||
            data_type_ == SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE ||
            data_type_ == SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE) {
          return s;
        } else {
          should_read_times_ = false;
          --current_;
          return Next();
        }
      }
    }

    if (times_reader_.ReadLine(&line)) {
      StatusOr<double> status_or =
          TryConvertToDouble(line, path_to_times_, current);
      if (!status_or.ok()) return status_or.status();
      timestamp = base::TimeDelta::FromSecondsD(status_or.ValueOrDie());
      sensor_data.set_timestamp(timestamp);
    }
  }

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(6) << current;
  std::string name = ss.str();
  switch (data_type_) {
    case SensorData::DATA_TYPE_LEFT_CAMERA:
    case SensorData::DATA_TYPE_RIGHT_CAMERA:
    case SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE:
    case SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE: {
      Image image;
      PixelFormat pixel_format;
      if (data_type_ == SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE ||
          data_type_ == SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE) {
        pixel_format = PIXEL_FORMAT_Y8;
      } else {
        pixel_format = PIXEL_FORMAT_BGR;
      }
      Status s =
          image.Load(path_to_data_.AppendASCII(name).AddExtensionASCII(".png"),
                     pixel_format);
      if (!s.ok()) return s;
      drivers::CameraFrame camera_frame(std::move(image), 0,
                                        sensor_data.timestamp());
      if (data_type_ == SensorData::DATA_TYPE_LEFT_CAMERA ||
          data_type_ == SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE) {
        sensor_data.set_left_camera_frame(std::move(camera_frame));
      } else {
        sensor_data.set_right_camera_frame(std::move(camera_frame));
      }
      break;
    }
    case SensorData::DATA_TYPE_POINTCLOUD: {
      map::Pointcloud pointcloud;
      Status s = pointcloud.Load(
          path_to_data_.AppendASCII(name).AddExtensionASCII(".bin"));
      if (!s.ok()) return s;
      sensor_data.set_pointcloud(std::move(pointcloud));
      break;
    }
    case SensorData::DATA_TYPE_GROUND_TRUTH_POSE: {
      if (!pose_reader_.IsOpened()) {
        Status s = pose_reader_.Open(path_to_data_, " ");
        if (!s.ok()) return s;
      }
      std::vector<std::string> rows;
      float v[12] = {
          0,
      };
      if (pose_reader_.ReadRows(&rows)) {
        for (int i = 0; i < 12; ++i) {
          StatusOr<double> status_or =
              TryConvertToDouble(rows[i], path_to_data_, current);
          if (!status_or.ok()) return status_or.status();
          v[i] = status_or.ValueOrDie();
        }
      }
      Eigen::Matrix3f R;
      R << v[0], v[1], v[2], v[4], v[5], v[6], v[8], v[9], v[10];
      Eigen::Quaternionf q(R);
      Pose3f pose;
      pose.set_orientation(Quaternionf{q.x(), q.y(), q.z(), q.w()});
      pose.set_position(Point3f{v[3], v[7], v[11]});
      sensor_data.set_pose(pose);
      break;
    }
    default:
      break;
  }
  return sensor_data;
}

bool KittiDatasetLoader::State::End() const {
  return times_reader_.IsOpened() && times_reader_.eof();
}

int KittiDatasetLoader::State::SkipHeadCountForCalibs() const {
  switch (data_type_) {
    case SensorData::DATA_TYPE_LEFT_CAMERA:
      return 0;
    case SensorData::DATA_TYPE_RIGHT_CAMERA:
      return 1;
    case SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE:
      return 2;
    case SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE:
      return 3;
    default:
      NOTREACHED();
      return 0;
  }
}

base::FilePath KittiDatasetLoader::State::PathToData() const {
  switch (data_type_) {
    case SensorData::DATA_TYPE_LEFT_CAMERA:
      return path_.AppendASCII("image_2");
    case SensorData::DATA_TYPE_RIGHT_CAMERA:
      return path_.AppendASCII("image_3");
    case SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE:
      return path_.AppendASCII("image_0");
    case SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE:
      return path_.AppendASCII("image_1");
    case SensorData::DATA_TYPE_POINTCLOUD:
      return path_.AppendASCII("velodyne");
    case SensorData::DATA_TYPE_GROUND_TRUTH_POSE:
      return path_;
    default:
      NOTREACHED();
      return path_;
  }
}

KittiDatasetLoader::KittiDatasetLoader(const base::FilePath& path,
                                       int data_types) {
  for (int i = 0;
       static_cast<SensorData::DataType>(1 << i) != SensorData::DATA_TYPE_END;
       ++i) {
    SensorData::DataType data_type = static_cast<SensorData::DataType>(1 << i);
    if (data_types & data_type) {
      states_.emplace_back(new State(data_type, path));
    }
  }
}

StatusOr<SensorMetaData> KittiDatasetLoader::Init(int data_type) {
  CHECK(base::bits::IsPowerOfTwo(data_type));
  for (auto& state : states_) {
    if (state->data_type() == static_cast<SensorData::DataType>(data_type)) {
      return state->Init();
    }
  }
  return errors::InvalidArgument("No state correspondent to data_type.");
}

StatusOr<SensorData> KittiDatasetLoader::Next(int data_type) {
  CHECK(base::bits::IsPowerOfTwo(data_type));
  for (auto& state : states_) {
    if (state->data_type() == static_cast<SensorData::DataType>(data_type)) {
      return state->Next();
    }
  }
  return errors::InvalidArgument("No state correspondent to data_type.");
}

bool KittiDatasetLoader::End(int data_type) const {
  CHECK(base::bits::IsPowerOfTwo(data_type));
  for (auto& state : states_) {
    if (state->data_type() == static_cast<SensorData::DataType>(data_type)) {
      return state->End();
    }
  }
  return true;
}

}  // namespace slam
}  // namespace felicia