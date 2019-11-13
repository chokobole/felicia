// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/slam/dataset/tum_dataset_loader.h"

#include "third_party/chromium/base/bits.h"
#include "third_party/chromium/base/strings/utf_string_conversions.h"

#include "felicia/slam/dataset/dataset_utils.h"

namespace felicia {
namespace slam {

TumDatasetLoader::State::State(SensorData::DataType data_type, TumKind tum_kind,
                               const base::FilePath& path)
    : tum_kind_(tum_kind),
      data_type_(data_type),
      path_(path),
      path_to_data_(PathToData()) {}

SensorData::DataType TumDatasetLoader::State::data_type() const {
  return data_type_;
}

StatusOr<SensorMetaData> TumDatasetLoader::State::Init() {
  SensorMetaData sensor_meta_data;
  if (tum_kind_ == FR1) {
    sensor_meta_data.set_left_K(EigenCameraMatrixd(517.3, 516.5, 318.6, 255.3));
    sensor_meta_data.set_left_D(
        EigenDistortionMatrixd(0.2624, -0.9531, -0.0054, 0.0026, 1.1633));
  } else if (tum_kind_ == FR2) {
    sensor_meta_data.set_left_K(EigenCameraMatrixd(520.9, 521.0, 325.1, 249.7));
    sensor_meta_data.set_left_D(
        EigenDistortionMatrixd(0.2312, -0.7849, -0.0033, -0.0001, 0.9172));
  } else if (tum_kind_ == FR3) {
    sensor_meta_data.set_left_K(EigenCameraMatrixd(535.4, 539.2, 320.1, 247.6));
    sensor_meta_data.set_left_D(
        EigenDistortionMatrixd(0.0, 0.0, 0.0, 0.0, 0.0));
  }

  return sensor_meta_data;
}

StatusOr<SensorData> TumDatasetLoader::State::Next() {
  if (!data_reader_.IsOpened()) {
    Status s = data_reader_.Open(path_to_data_, " ", 3);
    if (!s.ok()) return s;
  }

  ++current_;
  int current_line = current_ + 3;
  std::vector<std::string> rows;
  SensorData sensor_data;
  if (data_reader_.ReadRows(&rows)) {
    if (rows.size() != static_cast<size_t>(ColumnsForData())) {
      return errors::InvalidArgument(base::StringPrintf(
          "The number of columns is not valid at %" PRFilePath ":%d",
          path_to_data_.value().c_str(), current_line));
    }
    StatusOr<double> status_or =
        TryConvertToDouble(rows[0], path_to_data_, current_line);
    if (!status_or.ok()) return status_or.status();
    base::TimeDelta timestamp =
        base::TimeDelta::FromSecondsD(status_or.ValueOrDie());
    sensor_data.set_timestamp(timestamp);
    switch (data_type_) {
      case SensorData::DATA_TYPE_LEFT_CAMERA: {
        Image image;
        Status s = image.Load(path_.AppendASCII(rows[1]), PIXEL_FORMAT_BGR);
        if (!s.ok()) return s;
        drivers::CameraFrame camera_frame(std::move(image), 0, timestamp);
        sensor_data.set_left_camera_frame(std::move(camera_frame));
        break;
      }
      case SensorData::DATA_TYPE_DEPTH_CAMERA: {
        Image image;
        Status s = image.Load(path_.AppendASCII(rows[1]), PIXEL_FORMAT_Z16);
        if (!s.ok()) return s;
        drivers::CameraFrame camera_frame(std::move(image), 0, timestamp);
        drivers::DepthCameraFrame depth_camera_frame(std::move(camera_frame), 1,
                                                     30000);
        sensor_data.set_depth_camera_frame(std::move(depth_camera_frame));
        break;
      }
      case SensorData::DATA_TYPE_ACCELERATION: {
        float v[3];
        for (int i = 0; i < 3; ++i) {
          status_or =
              TryConvertToDouble(rows[i + 1], path_to_data_, current_line);
          if (!status_or.ok()) return status_or.status();
          v[i] = status_or.ValueOrDie();
        }
        sensor_data.set_acceleration(Vector3f{v[0], v[1], v[2]});
        break;
      }
      case SensorData::DATA_TYPE_GROUND_TRUTH_POSE: {
        float v[7];
        for (int i = 0; i < 7; ++i) {
          status_or =
              TryConvertToDouble(rows[i + 1], path_to_data_, current_line);
          if (!status_or.ok()) return status_or.status();
          v[i] = status_or.ValueOrDie();
        }
        Point3f p(v[0], v[1], v[2]);
        Quaternionf q(v[3], v[4], v[5], v[6]);
        sensor_data.set_pose(Pose3f{p, q});
        break;
      }
      default:
        break;
    }
  }
  return sensor_data;
}

bool TumDatasetLoader::State::End() const {
  return data_reader_.IsOpened() && data_reader_.eof();
}

base::FilePath TumDatasetLoader::State::PathToData() const {
  switch (data_type_) {
    case SensorData::DATA_TYPE_LEFT_CAMERA:
      return path_.AppendASCII("rgb.txt");
    case SensorData::DATA_TYPE_DEPTH_CAMERA:
      return path_.AppendASCII("depth.txt");
    case SensorData::DATA_TYPE_ACCELERATION:
      return path_.AppendASCII("accelerometer.txt");
    case SensorData::DATA_TYPE_GROUND_TRUTH_POSE:
      return path_.AppendASCII("groundtruth.txt");
    default:
      NOTREACHED();
      return path_;
  }
}

int TumDatasetLoader::State::ColumnsForData() const {
  switch (data_type_) {
    case SensorData::DATA_TYPE_LEFT_CAMERA:
      return 2;
    case SensorData::DATA_TYPE_DEPTH_CAMERA:
      return 2;
    case SensorData::DATA_TYPE_ACCELERATION:
      return 4;
    case SensorData::DATA_TYPE_GROUND_TRUTH_POSE:
      return 8;
    default:
      NOTREACHED();
      return 0;
  }
}

TumDatasetLoader::TumDatasetLoader(const base::FilePath& path, int data_types) {
  base::FilePath::StringType path_string = path.value();
  size_t pos = path_string.find(FILE_PATH_LITERAL("rgbd_dataset_freiburg"));
  TumKind tum_kind = FR1;
  if (path_string[pos + 21] == '2') {
    tum_kind = FR2;
  } else if (path_string[pos + 21] == '3') {
    tum_kind = FR3;
  }
  InitStates(path, tum_kind, data_types);
}

TumDatasetLoader::TumDatasetLoader(const base::FilePath& path, TumKind tum_kind,
                                   int data_types) {
  InitStates(path, tum_kind, data_types);
}

StatusOr<SensorMetaData> TumDatasetLoader::Init(int data_type) {
  CHECK(base::bits::IsPowerOfTwo(data_type));
  for (auto& state : states_) {
    if (state->data_type() == static_cast<SensorData::DataType>(data_type)) {
      return state->Init();
    }
  }
  return errors::InvalidArgument("No state correspondent to data_type.");
}

StatusOr<SensorData> TumDatasetLoader::Next(int data_type) {
  CHECK(base::bits::IsPowerOfTwo(data_type));
  for (auto& state : states_) {
    if (state->data_type() == static_cast<SensorData::DataType>(data_type)) {
      return state->Next();
    }
  }
  return errors::InvalidArgument("No state correspondent to data_type.");
}

bool TumDatasetLoader::End(int data_type) const {
  CHECK(base::bits::IsPowerOfTwo(data_type));
  for (auto& state : states_) {
    if (state->data_type() == static_cast<SensorData::DataType>(data_type)) {
      return state->End();
    }
  }
  return true;
}

void TumDatasetLoader::InitStates(const base::FilePath& path, TumKind tum_kind,
                                  int data_types) {
  for (int i = 0;
       static_cast<SensorData::DataType>(1 << i) != SensorData::DATA_TYPE_END;
       ++i) {
    SensorData::DataType data_type = static_cast<SensorData::DataType>(1 << i);
    if (data_types & data_type) {
      states_.emplace_back(new State(data_type, tum_kind, path));
    }
  }
}

}  // namespace slam
}  // namespace felicia