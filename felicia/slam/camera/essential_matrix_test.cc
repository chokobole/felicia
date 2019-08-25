#include "felicia/slam/camera/essential_matrix.h"

#include "gtest/gtest.h"

#include "felicia/core/lib/math/math_util.h"
#include "felicia/core/lib/unit/geometry/native_matrix_reference_test_util.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

TEST(EssentialMatrixTest, ComputeEssentialMatrix) {
  Eigen::AngleAxisf angle_axis(degree_to_radian(30.0),
                               Eigen::Vector3f(1, 0, 0));
  Eigen::Translation3f translation(1, 2, 3);
  Eigen::Matrix3f R = angle_axis.matrix();
  Eigen::Vector3f t = translation.vector();
  Eigen::Matrix3f expected = EigenEssentialMatrixf::ComputeFrom(R, t).matrix();
  {
    RigidBodyTransform3<Eigen::AngleAxisf, Eigen::Translation3f> transform(
        angle_axis, translation);
    ExpectEqualMatrix(expected,
                      EigenEssentialMatrixf::ComputeFrom(transform).matrix());
  }

  Eigen::Quaternionf quaternion(angle_axis);
  // Has to check if its matrix type are really same.
  ExpectEqualMatrix(angle_axis.matrix(), quaternion.matrix());
  ExpectEqualMatrix(
      expected,
      EigenEssentialMatrixf::ComputeFrom(quaternion.matrix(), t).matrix());
  {
    RigidBodyTransform3<Eigen::Quaternionf, Eigen::Translation3f> transform(
        quaternion, translation);
    ExpectEqualMatrix(expected,
                      EigenEssentialMatrixf::ComputeFrom(transform).matrix());
  }

#if defined(HAS_OPENCV)
  EigenMatrix3fRef R_ref(R);
  EigenVector3fRef t_ref(t);
  cv::Mat1f cv_mat_R;
  R_ref.ToCvMatrix(&cv_mat_R);
  cv::Mat1f cv_mat_t;
  t_ref.ToCvMatrix(&cv_mat_t);
  ExpectEqualMatrix(
      expected, CvEssentialMatrixf::ComputeFrom(cv_mat_R, cv_mat_t).matrix());
  {
    RigidBodyTransform3<cv::Mat1f, cv::Mat1f> transform(cv_mat_R, cv_mat_t);
    ExpectEqualMatrix(expected,
                      CvEssentialMatrixf::ComputeFrom(transform).matrix());
  }

  cv::Matx33f cv_matx33_R;
  R_ref.ToCvMatrix(&cv_mat_R);
  cv::Matx31f cv_matx31_t;
  t_ref.ToCvMatrix(&cv_matx31_t);
  ExpectEqualMatrix(
      expected,
      EssentialMatrix<cv::Matx33f>::ComputeFrom(cv_mat_R, cv_mat_t).matrix());
  {
    RigidBodyTransform3<cv::Matx33f, cv::Matx31f> transform(cv_mat_R, cv_mat_t);
    ExpectEqualMatrix(
        expected,
        EssentialMatrix<cv::Matx33f>::ComputeFrom(transform).matrix());
  }
#endif
}

}  // namespace slam
}  // namespace felicia