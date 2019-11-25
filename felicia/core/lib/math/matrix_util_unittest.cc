// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/math/matrix_util.h"

#include "gtest/gtest.h"

#include "felicia/core/lib/unit/geometry/native_matrix_reference_test_util.h"

namespace felicia {

TEST(MatrixBinaryOperationTest, HStack) {
  {
    typedef Eigen::Matrix3f LHSType;
    typedef Eigen::Matrix<float, 3, 2> RHSType;
    typedef Eigen::Matrix<float, 3, 5> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::HStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::HStack(mat, vec));
  }

  {
    typedef Eigen::Matrix<float, 3, Eigen::Dynamic> LHSType;
    typedef Eigen::Matrix<float, 3, 2> RHSType;
    typedef Eigen::Matrix<float, 3, Eigen::Dynamic> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    mat.resize(3, 3);
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    expected.resize(3, 5);
    internal::HStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::HStack(mat, vec));
  }

  {
    typedef Eigen::Matrix3f LHSType;
    typedef Eigen::Matrix<float, 3, Eigen::Dynamic> RHSType;
    typedef Eigen::Matrix<float, 3, Eigen::Dynamic> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    InitMatrix(1, 1, &to, &mat);
    vec.resize(3, 2);
    InitMatrix(to, 1, &to, &vec);
    expected.resize(3, 5);
    internal::HStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::HStack(mat, vec));
  }

  {
    typedef Eigen::Matrix3f LHSType;
    typedef Eigen::Matrix3f RHSType;
    typedef Eigen::Matrix<float, 3, 6> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::HStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::HStack(mat, vec));
  }

#if defined(HAS_OPENCV)
  {
    typedef cv::Matx33f LHSType;
    typedef cv::Matx<float, 3, 2> RHSType;
    typedef cv::Matx<float, 3, 5> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::HStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::HStack(mat, vec));
  }

  {
    typedef cv::Mat1f LHSType;
    typedef cv::Matx<float, 3, 2> RHSType;
    typedef cv::Mat1f ReturnType;
    LHSType mat(3, 3);
    RHSType vec;
    ReturnType expected(3, 5);
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::HStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::HStack(mat, vec));
  }

  {
    typedef cv::Matx33f LHSType;
    typedef cv::Mat1f RHSType;
    typedef cv::Mat1f ReturnType;
    LHSType mat;
    RHSType vec(3, 2);
    ReturnType expected(3, 5);
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::HStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::HStack(mat, vec));
  }

  {
    typedef cv::Mat1f LHSType;
    typedef cv::Mat1f RHSType;
    typedef cv::Mat1f ReturnType;
    LHSType mat(3, 3);
    RHSType vec(3, 2);
    ReturnType expected(3, 5);
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::HStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::HStack(mat, vec));
  }

  {
    typedef cv::Matx33f LHSType;
    typedef cv::Matx33f RHSType;
    typedef cv::Matx<float, 3, 6> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::HStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::HStack(mat, vec));
  }
#endif
}

TEST(MatrixBinaryOperationTest, VStack) {
  {
    typedef Eigen::Matrix3f LHSType;
    typedef Eigen::Matrix<float, 2, 3> RHSType;
    typedef Eigen::Matrix<float, 5, 3> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::VStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::VStack(mat, vec));
  }

  {
    typedef Eigen::Matrix<float, Eigen::Dynamic, 3> LHSType;
    typedef Eigen::Matrix<float, 2, 3> RHSType;
    typedef Eigen::Matrix<float, 5, Eigen::Dynamic> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    mat.resize(3, 3);
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    expected.resize(5, 3);
    internal::VStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::VStack(mat, vec));
  }

  {
    typedef Eigen::Matrix3f LHSType;
    typedef Eigen::Matrix<float, Eigen::Dynamic, 3> RHSType;
    typedef Eigen::Matrix<float, Eigen::Dynamic, 3> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    InitMatrix(1, 1, &to, &mat);
    vec.resize(2, 3);
    InitMatrix(to, 1, &to, &vec);
    expected.resize(5, 3);
    internal::VStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::VStack(mat, vec));
  }

  {
    typedef Eigen::Matrix3f LHSType;
    typedef Eigen::Matrix3f RHSType;
    typedef Eigen::Matrix<float, 6, 3> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::VStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::VStack(mat, vec));
  }

#if defined(HAS_OPENCV)
  {
    typedef cv::Matx33f LHSType;
    typedef cv::Matx<float, 2, 3> RHSType;
    typedef cv::Matx<float, 5, 3> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::VStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::VStack(mat, vec));
  }

  {
    typedef cv::Mat1f LHSType;
    typedef cv::Matx<float, 2, 3> RHSType;
    typedef cv::Mat1f ReturnType;
    LHSType mat(3, 3);
    RHSType vec;
    ReturnType expected(5, 3);
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::VStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::VStack(mat, vec));
  }

  {
    typedef cv::Matx33f LHSType;
    typedef cv::Mat1f RHSType;
    typedef cv::Mat1f ReturnType;
    LHSType mat;
    RHSType vec(2, 3);
    ReturnType expected(5, 3);
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::VStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::VStack(mat, vec));
  }

  {
    typedef cv::Mat1f LHSType;
    typedef cv::Mat1f RHSType;
    typedef cv::Mat1f ReturnType;
    LHSType mat(3, 3);
    RHSType vec(2, 3);
    ReturnType expected(5, 3);
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::VStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::VStack(mat, vec));
  }

  {
    typedef cv::Matx33f LHSType;
    typedef cv::Matx33f RHSType;
    typedef cv::Matx<float, 6, 3> ReturnType;
    LHSType mat;
    RHSType vec;
    ReturnType expected;
    int to;
    InitMatrix(1, 1, &to, &mat);
    InitMatrix(to, 1, &to, &vec);
    internal::VStack(mat, vec, &expected);
    ExpectEqualMatrix(
        expected, MatrixBinaryOperation<LHSType, RHSType>::VStack(mat, vec));
  }
#endif
}

}  // namespace felicia