#ifndef FELICIA_SLAM_DATASET_RGBD_DATA_H_
#define FELICIA_SLAM_DATASET_RGBD_DATA_H_

#include <string>

#include "felicia/core/lib/base/export.h"

namespace felicia {
namespace slam {

struct EXPORT RgbdData {
  std::string color_image_filename;
  std::string depth_image_filename;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_RGBD_DATA_H_