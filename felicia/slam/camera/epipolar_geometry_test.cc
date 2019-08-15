#include "felicia/slam/camera/epipolar_geometry.h"

#include "gtest/gtest.h"

#include "felicia/core/lib/math/math_util.h"
#include "felicia/core/lib/unit/geometry/native_matrix_reference_test_util.h"

namespace felicia {
namespace slam {

TEST(EpipolarGeometryTest, ComputeEssentialMatrix) {
  Eigen::AngleAxisf angle_axis(degree_to_radian(30.0),
                               Eigen::Vector3f(1, 0, 0));
  Eigen::Translation3f translation(1, 2, 3);
  Eigen::Matrix3f R = angle_axis.matrix();
  Eigen::Vector3f t = translation.vector();
  auto expected = EpipolarGeometry::ComputeEssentialMatrix(R, t);
  {
    RigidBodyTransform3<Eigen::AngleAxisf, Eigen::Translation3f> transform(
        angle_axis, translation);
    ExpectEqualMatrix(expected,
                      EpipolarGeometry::ComputeEssentialMatrix(transform));
  }

  Eigen::Quaternionf quaternion(angle_axis);
  // Has to check if its matrix type are really same.
  ExpectEqualMatrix(angle_axis.matrix(), quaternion.matrix());
  ExpectEqualMatrix(expected, EpipolarGeometry::ComputeEssentialMatrix(
                                  quaternion.matrix(), t));
  {
    RigidBodyTransform3<Eigen::Quaternionf, Eigen::Translation3f> transform(
        quaternion, translation);
    ExpectEqualMatrix(expected,
                      EpipolarGeometry::ComputeEssentialMatrix(transform));
  }

#if defined(HAS_OPENCV)
  EigenMatrix3fRef R_ref(R);
  EigenVector3fRef t_ref(t);
  cv::Matx33f cv_matx33_R;
  R_ref.ToCvMatrix(&cv_matx33_R);
  cv::Matx31f cv_matx31_t;
  t_ref.ToCvMatrix(&cv_matx31_t);
  ExpectEqualMatrix(expected, EpipolarGeometry::ComputeEssentialMatrix(
                                  cv_matx33_R, cv_matx31_t));
  {
    RigidBodyTransform3<cv::Matx33f, cv::Matx31f> transform(cv_matx33_R,
                                                            cv_matx31_t);
    ExpectEqualMatrix(expected,
                      EpipolarGeometry::ComputeEssentialMatrix(transform));
  }
#endif
}

}  // namespace slam
}  // namespace felicia