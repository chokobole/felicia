#include "felicia/slam/camera/epipolar_geometry.h"

#include "gtest/gtest.h"

#include "third_party/chromium/base/logging.h"

#include "felicia/core/lib/math/math_util.h"
#include "felicia/core/lib/unit/geometry/native_matrix_reference.h"

namespace felicia {
namespace slam {

void ExpectFloatEqualMatrix(const Eigen::Matrix3f& m,
                            const Eigen::Matrix3f& m2) {
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      EXPECT_FLOAT_EQ(m(i, j), m2(i, j));
    }
  }
}

#if defined(HAS_OPENCV)
void ExpectFloatEqualMatrix(const Eigen::Matrix3f& m, const cv::Matx33f& m2) {
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      EXPECT_FLOAT_EQ(m(i, j), m2(i, j));
    }
  }
}
#endif

TEST(EpipolarGeometryTest, ComputeEssentialMatrix) {
  Eigen::AngleAxisf angle_axis(degree_to_radian(30.0),
                               Eigen::Vector3f(1, 0, 0));
  Eigen::Translation3f translation(1, 2, 3);
  Eigen::Matrix3f R = angle_axis.matrix();
  Eigen::Vector3f t = translation.vector();
  auto expected = EpiploarGeometry::ComputeEssentialMatrix(R, t);
  {
    FactorableNativeTransform3<Eigen::AngleAxisf, Eigen::Translation3f>
        transform(angle_axis, translation);
    ExpectFloatEqualMatrix(expected,
                           EpiploarGeometry::ComputeEssentialMatrix(transform));
  }

  Eigen::Quaternionf quaternion(angle_axis);
  // Has to check if its matrix type are really same.
  ExpectFloatEqualMatrix(angle_axis.matrix(), quaternion.matrix());
  ExpectFloatEqualMatrix(expected, EpiploarGeometry::ComputeEssentialMatrix(
                                       quaternion.matrix(), t));
  {
    FactorableNativeTransform3<Eigen::Quaternionf, Eigen::Translation3f>
        transform(quaternion, translation);
    ExpectFloatEqualMatrix(expected,
                           EpiploarGeometry::ComputeEssentialMatrix(transform));
  }

#if defined(HAS_OPENCV)
  EigenMatrix3fRef R_ref(R);
  EigenVector3fRef t_ref(t);
  cv::Matx33f cv_matx33_R;
  R_ref.ToCvMatrix(&cv_matx33_R);
  cv::Matx31f cv_matx31_t;
  t_ref.ToCvMatrix(&cv_matx31_t);
  ExpectFloatEqualMatrix(expected, EpiploarGeometry::ComputeEssentialMatrix(
                                       cv_matx33_R, cv_matx31_t));
  {
    FactorableNativeTransform3<cv::Matx33f, cv::Matx31f> transform(cv_matx33_R,
                                                                   cv_matx31_t);
    ExpectFloatEqualMatrix(expected,
                           EpiploarGeometry::ComputeEssentialMatrix(transform));
  }
#endif
}

}  // namespace slam
}  // namespace felicia