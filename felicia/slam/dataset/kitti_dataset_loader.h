#ifndef FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_
#define FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_

#include "Eigen/Core"

#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/file/buffered_reader.h"
#include "felicia/core/util/dataset/dataset_loader.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

struct KittiCalibData {
  EigenProjectionMatrixd p0;
  EigenProjectionMatrixd p1;
  EigenProjectionMatrixd p2;
  EigenProjectionMatrixd p3;
};

// TODO: We should read the image and give users a CameraFrame
struct KittiData {
  std::string left_image_filename;
  std::string right_image_filename;
  double timestamp;
};

class EXPORT KittiDatasetLoader
    : DatasetLoader<KittiCalibData, KittiData>::Delegate {
 public:
  KittiDatasetLoader(const base::FilePath& path_to_sequence, uint8_t sequence);

  // DatasetLoader<KittiCalibData, KittiData>::Delegate methods
  StatusOr<KittiCalibData> Init() override;
  StatusOr<KittiData> Next() override;
  bool End() const override;
  size_t length() const override;

 private:
  base::FilePath path_;
  BufferedReader times_reader_;
  base::FilePath path_to_left_image_;
  base::FilePath path_to_right_image_;
  size_t current_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_KITTI_DATASET_LOADER_H_