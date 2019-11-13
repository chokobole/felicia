// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_
#define FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/file/buffered_reader.h"
#include "felicia/core/lib/file/csv_reader.h"
#include "felicia/core/util/dataset/dataset_loader.h"
#include "felicia/slam/dataset/sensor_data.h"
#include "felicia/slam/dataset/sensor_meta_data.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

// SensorData::DataType data_type = SensorData::DATA_TYPE_LEFT_CAMERA;
// KittiDatasetLoader loader("/path/to/euroc", data_type);
// StatusOr<SensorMetaData> sensor_meta_data = loader.Init();
// StatusOr<SensorData> sensor_data = loader.Next();
// For example, /path/to/kitti should point to the
// /path/to/dataset/sequences/00. But exceptionally, for groundtruth pose,
// path/to/kitti should point to /path/to/dataset/poses/00.txt
class FEL_EXPORT KittiDatasetLoader
    : public DatasetLoader<SensorMetaData, SensorData>::Delegate {
 public:
  class State {
   public:
    State(SensorData::DataType data_type, const base::FilePath& path);

    SensorData::DataType data_type() const;

    StatusOr<SensorMetaData> Init();
    StatusOr<SensorData> Next();
    bool End() const;

   private:
    int SkipHeadCountForCalibs() const;
    base::FilePath PathToData() const;

    SensorData::DataType data_type_;
    // path to root
    base::FilePath path_;
    // path to root of each data e.g) /path/to/root/image0
    base::FilePath path_to_data_;
    // path to times.txt
    base::FilePath path_to_times_;
    CsvReader pose_reader_;
    BufferedReader times_reader_;
    bool should_read_times_ = true;
    size_t current_ = 0;

    DISALLOW_COPY_AND_ASSIGN(State);
  };

  KittiDatasetLoader(const base::FilePath& path, int data_types);

  // DatasetLoader<SensorMetaData, SensorData>::Delegate methods
  StatusOr<SensorMetaData> Init(int data_type) override;
  StatusOr<SensorData> Next(int data_type) override;
  bool End(int data_type) const override;

 private:
  std::vector<std::unique_ptr<State>> states_;

  DISALLOW_COPY_AND_ASSIGN(KittiDatasetLoader);
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_