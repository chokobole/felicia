#include "felicia/slam/dataset/kitti_dataset_loader.h"
#include "felicia/slam/dataset/euroc_dataset_loader.h"
#include "felicia/slam/dataset/tum_dataset_loader.h"

#include "gtest/gtest.h"

namespace felicia {
namespace slam {

// TEST(KittiDatasetLoaderTest, OpenTest) {
//   KittiDatasetLoader loader(
//       base::FilePath("/Users/chokobole/Workspace/data/kitti/"),
//       KittiDatasetLoader::GRAYSCALE);

//   StatusOr<SensorData> sensor_data = loader.Next();
//   LOG(INFO) << sensor_data.ValueOrDie().left_image_filename();
//   sensor_data = loader.Next();
//   LOG(INFO) << sensor_data.ValueOrDie().left_image_filename();
//   sensor_data = loader.Next();
//   LOG(INFO) << sensor_data.ValueOrDie().left_image_filename();
// }

TEST(TumDatasetLoaderTest, OpenTest) {
  TumDatasetLoader loader(
      base::FilePath(
          "/Users/chokobole/Workspace/data/rgbd_dataset_freiburg1_xyz"),
      TumDatasetLoader::RGBD);

  StatusOr<SensorData> sensor_data = loader.Next();
  LOG(INFO) << sensor_data.ValueOrDie().timestamp();
  sensor_data = loader.Next();
  LOG(INFO) << sensor_data.ValueOrDie().left_image_filename();
  sensor_data = loader.Next();
  LOG(INFO) << sensor_data.ValueOrDie().depth_image_filename();
}

TEST(EurocDatasetLoaderTest, OpenTest) {
  EurocDatasetLoader loader(
      base::FilePath("/Users/chokobole/Workspace/data/mav0"),
      EurocDatasetLoader::CAM0);

  StatusOr<SensorMetaData> sensor_meta_data = loader.Init();
  if (sensor_meta_data.ok())
    LOG(INFO) << sensor_meta_data.ValueOrDie().left_K().matrix();

  StatusOr<SensorData> sensor_data = loader.Next();
  LOG(INFO) << sensor_data.ValueOrDie().timestamp();
  sensor_data = loader.Next();
  LOG(INFO) << sensor_data.ValueOrDie().left_image_filename();
  sensor_data = loader.Next();
  LOG(INFO) << sensor_data.ValueOrDie().left_image_filename();
}

}  // namespace slam
}  // namespace felicia