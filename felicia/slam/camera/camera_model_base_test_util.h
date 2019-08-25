#ifndef FELICIA_SLAM_CAMERA_CAMERA_MODEL_BASE_TEST_UTIL_H_
#define FELICIA_SLAM_CAMERA_CAMERA_MODEL_BASE_TEST_UTIL_H_

#include "felicia/core/lib/math/math_util.h"
#include "felicia/core/lib/unit/geometry/native_matrix_reference_test_util.h"
#include "felicia/slam/camera/stereo_camera_model_base.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

template <typename CameraModelType, typename CameraModelType2>
void ExpectEqualCameraModel(const CameraModelType& camera_model,
                            const CameraModelType2& camera_model2) {
  EXPECT_EQ(camera_model.image_size(), camera_model2.image_size());
  ExpectEqualMatrix(camera_model.K_raw().matrix(),
                    camera_model2.K_raw().matrix());
  ExpectEqualMatrix(camera_model.D_raw().matrix(),
                    camera_model2.D_raw().matrix());
  ExpectEqualMatrix(camera_model.R().matrix(), camera_model2.R().matrix());
  ExpectEqualMatrix(camera_model.P().matrix(), camera_model2.P().matrix());
}

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_CAMERA_CAMERA_MODEL_BASE_TEST_UTIL_H_