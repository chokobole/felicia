// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/slam/dataset/euroc_dataset_loader.h"

#include "third_party/chromium/base/bits.h"
#include "third_party/chromium/base/strings/utf_string_conversions.h"

#include "felicia/core/lib/file/yaml_reader.h"
#include "felicia/drivers/imu/imu_filter_factory.h"
#include "felicia/slam/dataset/dataset_utils.h"

namespace felicia {
namespace slam {

EurocDatasetLoader::State::State(SensorData::DataType data_type,
                                 const base::FilePath& path)
    : data_type_(data_type),
      path_(path),
      path_to_data_(PathToData()),
      path_to_data_list_(PathToDataList()) {}

SensorData::DataType EurocDatasetLoader::State::data_type() const {
  return data_type_;
}

StatusOr<SensorMetaData> EurocDatasetLoader::State::Init() {
  YamlReader reader;
  Status s = reader.Open(PathToMetaData());
  if (!s.ok()) return s;
  const YAML::Node& node = reader.node();
  SensorMetaData sensor_meta_data;
  switch (data_type_) {
    case SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE:
    case SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE: {
      YAML::Node intrinsics = node["intrinsics"];
      YAML::Node distortion_coeffs = node["distortion_coefficients"];
      EigenCameraMatrixd K(
          intrinsics[0].as<double>(), intrinsics[1].as<double>(),
          intrinsics[2].as<double>(), intrinsics[3].as<double>());
      EigenDistortionMatrixd D(
          distortion_coeffs[0].as<double>(), distortion_coeffs[1].as<double>(),
          distortion_coeffs[2].as<double>(), distortion_coeffs[3].as<double>(),
          DISTORTION_MODEL_PLUMB_BOB);
      if (data_type_ == SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE) {
        sensor_meta_data.set_left_K(K);
        sensor_meta_data.set_left_D(D);
      } else {
        sensor_meta_data.set_right_K(K);
        sensor_meta_data.set_right_D(D);
      }
      break;
    }
    // TODO: Fill the belows.
    case SensorData::DATA_TYPE_IMU:
    case SensorData::DATA_TYPE_GROUND_TRUTH_POSITION:
    case SensorData::DATA_TYPE_GROUND_TRUTH_POSE:
      return errors::Unimplemented("");
      break;
    default:
      break;
  }
  return sensor_meta_data;
}

StatusOr<SensorData> EurocDatasetLoader::State::Next() {
  if (!data_list_reader_.IsOpened()) {
    Status s = data_list_reader_.Open(path_to_data_list_, ",", 1);
    if (!s.ok()) return s;
  }

  ++current_;
  int current_line = current_ + 1;
  std::vector<std::string> rows;
  SensorData sensor_data;
  if (data_list_reader_.ReadRows(&rows)) {
    if (rows.size() != static_cast<size_t>(ColumnsForData())) {
      return errors::InvalidArgument(base::StringPrintf(
          "The number of columns is not valid at %" PRFilePath ":%d",
          path_to_data_list_.value().c_str(), current_line));
    }
    StatusOr<double> status_or =
        TryConvertToDouble(rows[0], path_to_data_list_, current_line);
    if (!status_or.ok()) return status_or.status();
    base::TimeDelta timestamp =
        base::TimeDelta::FromNanosecondsD(status_or.ValueOrDie());
    sensor_data.set_timestamp(timestamp);

    switch (data_type_) {
      case SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE:
      case SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE: {
        Image image;
        base::FilePath path = path_to_data_.AppendASCII(rows[1]);
        Status s = image.Load(path, PIXEL_FORMAT_Y8);
        if (!s.ok()) return s;
        drivers::CameraFrame camera_frame(std::move(image), LoadFrameRate(),
                                          timestamp);
        if (data_type_ == SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE) {
          sensor_data.set_left_camera_frame(std::move(camera_frame));
        } else {
          sensor_data.set_right_camera_frame(std::move(camera_frame));
        }
        break;
      }
      case SensorData::DATA_TYPE_IMU: {
        if (!imu_filter_.get()) {
          imu_filter_ = drivers::ImuFilterFactory::NewImuFilter(
              drivers::ImuFilterFactory::MADGWICK_FILTER_KIND);
        }

        float v[6];
        for (int i = 0; i < 6; ++i) {
          status_or =
              TryConvertToDouble(rows[i + 1], path_to_data_, current_line);
          if (!status_or.ok()) return status_or.status();
          v[i] = status_or.ValueOrDie();
        }
        drivers::ImuFrame imu_frame;
        Vector3f angular_velocity(v[0], v[1], v[2]);
        Vector3f linear_acceleration(v[3], v[4], v[5]);
        imu_frame.set_linear_acceleration(linear_acceleration);
        imu_frame.set_angular_velocity(angular_velocity);
        imu_filter_->UpdateLinearAcceleration(linear_acceleration);
        imu_filter_->UpdateAngularVelocity(angular_velocity, timestamp);
        imu_frame.set_orientation(imu_filter_->orientation());
        imu_frame.set_timestamp(timestamp);
        sensor_data.set_imu_frame(imu_frame);
        break;
      }
      case SensorData::DATA_TYPE_GROUND_TRUTH_POSITION: {
        float v[3];
        for (int i = 0; i < 3; ++i) {
          status_or =
              TryConvertToDouble(rows[i + 1], path_to_data_, current_line);
          if (!status_or.ok()) return status_or.status();
          v[i] = status_or.ValueOrDie();
        }
        sensor_data.set_position(Point3f{v[0], v[1], v[2]});
        break;
      }
      case SensorData::DATA_TYPE_GROUND_TRUTH_POSE: {
        float v[16];
        for (int i = 0; i < 16; ++i) {
          status_or =
              TryConvertToDouble(rows[i + 1], path_to_data_, current_line);
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
      default:
        break;
    }
  }
  return sensor_data;
}

bool EurocDatasetLoader::State::End() const {
  return data_list_reader_.IsOpened() && data_list_reader_.eof();
}

float EurocDatasetLoader::State::LoadFrameRate() {
  if (frame_rate_ != -1) return frame_rate_;
  YamlReader reader;
  Status s = reader.Open(PathToMetaData());
  if (!s.ok()) {
    frame_rate_ = 0;
  } else {
    const YAML::Node& node = reader.node();
    frame_rate_ = node["rate_hz"].as<float>();
  }
  return frame_rate_;
}

base::FilePath EurocDatasetLoader::State::PathToMetaData() const {
  return PathToDataType().AppendASCII("sensor.yaml");
}

base::FilePath EurocDatasetLoader::State::PathToData() const {
  return PathToDataType().AppendASCII("data");
}

base::FilePath EurocDatasetLoader::State::PathToDataList() const {
  return PathToDataType().AppendASCII("data.csv");
}

base::FilePath EurocDatasetLoader::State::PathToDataType() const {
  switch (data_type_) {
    case SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE:
      return path_.AppendASCII("cam0");
    case SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE:
      return path_.AppendASCII("cam1");
    case SensorData::DATA_TYPE_IMU:
      return path_.AppendASCII("imu0");
    case SensorData::DATA_TYPE_GROUND_TRUTH_POSITION:
      return path_.AppendASCII("leica0");
    case SensorData::DATA_TYPE_GROUND_TRUTH_POSE:
      return path_.AppendASCII("state_groundtruth_estimate0");
    default:
      NOTREACHED();
      break;
  }
  return path_;
}

int EurocDatasetLoader::State::ColumnsForData() const {
  switch (data_type_) {
    case SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE:
      return 2;
    case SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE:
      return 2;
    case SensorData::DATA_TYPE_IMU:
      return 7;
    case SensorData::DATA_TYPE_GROUND_TRUTH_POSITION:
      return 4;
    case SensorData::DATA_TYPE_GROUND_TRUTH_POSE:
      return 17;
    default:
      NOTREACHED();
      break;
  }
  return 0;
}

EurocDatasetLoader::EurocDatasetLoader(const base::FilePath& path,
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

StatusOr<SensorMetaData> EurocDatasetLoader::Init(int data_type) {
  CHECK(base::bits::IsPowerOfTwo(data_type));
  for (auto& state : states_) {
    if (state->data_type() == static_cast<SensorData::DataType>(data_type)) {
      return state->Init();
    }
  }
  return errors::InvalidArgument("No state correspondent to data_type.");
}

StatusOr<SensorData> EurocDatasetLoader::Next(int data_type) {
  CHECK(base::bits::IsPowerOfTwo(data_type));
  for (auto& state : states_) {
    if (state->data_type() == static_cast<SensorData::DataType>(data_type)) {
      return state->Next();
    }
  }
  return errors::InvalidArgument("No state correspondent to data_type.");
}

bool EurocDatasetLoader::End(int data_type) const {
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