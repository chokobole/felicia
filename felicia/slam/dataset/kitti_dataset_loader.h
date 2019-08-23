#ifndef FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_
#define FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/file/buffered_reader.h"
#include "felicia/core/util/dataset/dataset_loader.h"
#include "felicia/slam/dataset/sensor_data.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

struct KittiCalibData {
  EigenProjectionMatrixd p0;
  EigenProjectionMatrixd p1;
  EigenProjectionMatrixd p2;
  EigenProjectionMatrixd p3;
};

// KittiDatasetLoader loader("/path/to/kitti", 0);
// StatusOr<KittiCalibData> status_or = loader.Init();
// StatusOr<SensorData> status_or2 = loader.Next();
// For example, /path/to/kitti points to the /path/to/dataset/sequences.
class EXPORT KittiDatasetLoader
    : DatasetLoader<KittiCalibData, SensorData>::Delegate {
 public:
  KittiDatasetLoader(const base::FilePath& path_to_sequence, uint8_t sequence);

  // DatasetLoader<KittiCalibData, SensorData>::Delegate methods
  StatusOr<KittiCalibData> Init() override;
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
  size_t current_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_