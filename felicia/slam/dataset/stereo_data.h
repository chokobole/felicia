#ifndef FELICIA_SLAM_DATASET_STEREO_DATA_H_
#define FELICIA_SLAM_DATASET_STEREO_DATA_H_

#include <string>

#include "felicia/core/lib/base/export.h"

namespace felicia {
namespace slam {

struct EXPORT StereoData {
  std::string left_image_filename;
  std::string right_image_filename;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_STEREO_DATA_H_