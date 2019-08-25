#ifndef FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_
#define FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/file/buffered_reader.h"
#include "felicia/core/util/dataset/dataset_loader.h"
#include "felicia/slam/dataset/sensor_data.h"
#include "felicia/slam/dataset/sensor_meta_data.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

// KittiDatasetLoader loader("/path/to/kitti");
// StatusOr<SensorMetaData> sensor_meta_data = loader.Init();
// StatusOr<SensorData> sensor_data = loader.Next();
// For example, /path/to/kitti points to the /path/to/dataset/sequences/00.
class EXPORT KittiDatasetLoader
    : public DatasetLoader<SensorMetaData, SensorData>::Delegate {
 public:
  enum DataKind { GRAYSCALE, COLOR, VELODYNE_LASER_DATA, GROUND_TRUTH };

  KittiDatasetLoader(const base::FilePath& path, DataKind data_kind);

  // DatasetLoader<SensorMetaData, SensorData>::Delegate methods
  StatusOr<SensorMetaData> Init() override;
  StatusOr<SensorData> Next() override;
  bool End() const override;

 private:
  // path to calib.txt
  base::FilePath calibs_path_;
  // path to times.txt
  base::FilePath times_path_;
  // path to root of left images
  base::FilePath left_images_path_;
  // path to root of right images
  base::FilePath right_images_path_;
  BufferedReader times_reader_;
  DataKind data_kind_;
  size_t current_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_