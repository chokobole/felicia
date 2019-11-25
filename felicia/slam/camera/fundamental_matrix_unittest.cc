// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/slam/camera/essential_matrix.h"

#include "gtest/gtest.h"

#include "felicia/core/lib/math/math_util.h"
#include "felicia/core/lib/unit/geometry/native_matrix_reference_test_util.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

TEST(FundamentalMatrixTest, ComputeFundamentalMatrix) {
  Eigen::AngleAxisf angle_axis(degree_to_radian(30.0),
                               Eigen::Vector3f(1, 0, 0));
  Eigen::Translation3f translation(1, 2, 3);
  Eigen::Matrix3f R = angle_axis.matrix();
  Eigen::Vector3f t = translation.vector();
  Eigen::Matrix3f K;
  K << 300, 0, 320, 0, 300, 240, 0, 0, 1;
  Eigen::Matrix3f expected =
      EigenFundamentalMatrixf::ComputeFrom(K, K, R, t).matrix();
  {
    RigidBodyTransform3<Eigen::AngleAxisf, Eigen::Translation3f> transform(
        angle_axis, translation);
    ExpectEqualMatrix(
        expected,
        EigenFundamentalMatrixf::ComputeFrom(K, K, transform).matrix());
  }

  Eigen::Quaternionf quaternion(angle_axis);
  // Has to check if its matrix type are really same.
  ExpectEqualMatrix(angle_axis.matrix(), quaternion.matrix());
  ExpectEqualMatrix(expected, EigenFundamentalMatrixf::ComputeFrom(
                                  K, K, quaternion.matrix(), t)
                                  .matrix());
  {
    RigidBodyTransform3<Eigen::Quaternionf, Eigen::Translation3f> transform(
        quaternion, translation);
    ExpectEqualMatrix(
        expected,
        EigenFundamentalMatrixf::ComputeFrom(K, K, transform).matrix());
  }

#if defined(HAS_OPENCV)
  EigenMatrix3fRef R_ref(R);
  EigenVector3fRef t_ref(t);
  EigenMatrix3fRef K_ref(K);
  cv::Mat1f cv_mat_R;
  R_ref.ToCvMatrix(&cv_mat_R);
  cv::Mat1f cv_mat_t;
  t_ref.ToCvMatrix(&cv_mat_t);
  cv::Mat1f cv_mat_K;
  K_ref.ToCvMatrix(&cv_mat_K);
  ExpectEqualMatrix(expected, CvFundamentalMatrixf::ComputeFrom(
                                  cv_mat_K, cv_mat_K, cv_mat_R, cv_mat_t)
                                  .matrix());
  {
    RigidBodyTransform3<cv::Mat1f, cv::Mat1f> transform(cv_mat_R, cv_mat_t);
    ExpectEqualMatrix(expected, CvFundamentalMatrixf::ComputeFrom(
                                    cv_mat_K, cv_mat_K, transform)
                                    .matrix());
  }

  cv::Matx33f cv_matx33_R;
  R_ref.ToCvMatrix(&cv_mat_R);
  cv::Matx31f cv_matx31_t;
  t_ref.ToCvMatrix(&cv_matx31_t);
  cv::Matx33f cv_matx33_K;
  K_ref.ToCvMatrix(&cv_matx33_K);
  ExpectEqualMatrix(expected, FundamentalMatrix<cv::Matx33f>::ComputeFrom(
                                  cv_matx33_K, cv_matx33_K, cv_mat_R, cv_mat_t)
                                  .matrix());
  {
    RigidBodyTransform3<cv::Matx33f, cv::Matx31f> transform(cv_mat_R, cv_mat_t);
    ExpectEqualMatrix(expected, FundamentalMatrix<cv::Matx33f>::ComputeFrom(
                                    cv_matx33_K, cv_matx33_K, transform)
                                    .matrix());
  }
#endif
}

}  // namespace slam
}  // namespace felicia