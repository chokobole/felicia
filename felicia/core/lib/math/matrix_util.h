#ifndef FELICIA_CORE_LIB_MATH_MATRIX_UTIL_H_
#define FELICIA_CORE_LIB_MATH_MATRIX_UTIL_H_

#include "Eigen/Core"

#include "felicia/core/lib/unit/geometry/native_matrix_reference.h"

#if defined(HAS_OPENCV)
#include <opencv2/opencv.hpp>
#endif

namespace felicia {

constexpr int SumOrDynamic(int x, int y) {
  if (x == Eigen::Dynamic || y == Eigen::Dynamic) return Eigen::Dynamic;
  return x + y;
}

namespace internal {

template <typename MatrixType, typename MatrixType2, typename MatrixType3>
void HStack(const MatrixType& lhs, const MatrixType2& rhs, MatrixType3* out) {
  typedef ConstNativeMatrixRef<MatrixType> ConstMatrixTypeRef;
  typedef ConstNativeMatrixRef<MatrixType2> ConstMatrixTypeRef2;
  typedef NativeMatrixRef<MatrixType3> MatrixTypeRef3;
  ConstMatrixTypeRef lhs_ref(lhs);
  ConstMatrixTypeRef2 rhs_ref(rhs);
  MatrixTypeRef3 out_ref(*out);
  for (int i = 0; i < lhs_ref.rows(); ++i) {
    for (int j = 0; j < lhs_ref.cols(); ++j) {
      out_ref.at(i, j) = lhs_ref.at(i, j);
    }
  }

  for (int i = 0; i < rhs_ref.rows(); ++i) {
    for (int j = 0; j < rhs_ref.cols(); ++j) {
      out_ref.at(i, lhs_ref.cols() + j) = rhs_ref.at(i, j);
    }
  }
}

template <typename MatrixType, typename MatrixType2, typename MatrixType3>
void VStack(const MatrixType& lhs, const MatrixType2& rhs, MatrixType3* out) {
  typedef ConstNativeMatrixRef<MatrixType> ConstMatrixTypeRef;
  typedef ConstNativeMatrixRef<MatrixType2> ConstMatrixTypeRef2;
  typedef NativeMatrixRef<MatrixType3> MatrixTypeRef3;
  ConstMatrixTypeRef lhs_ref(lhs);
  ConstMatrixTypeRef2 rhs_ref(rhs);
  MatrixTypeRef3 out_ref(*out);
  for (int i = 0; i < lhs_ref.rows(); ++i) {
    for (int j = 0; j < lhs_ref.cols(); ++j) {
      out_ref.at(i, j) = lhs_ref.at(i, j);
    }
  }

  for (int i = 0; i < rhs_ref.rows(); ++i) {
    for (int j = 0; j < rhs_ref.cols(); ++j) {
      out_ref.at(lhs_ref.rows() + i, j) = rhs_ref.at(i, j);
    }
  }
}

}  // namespace internal

template <typename LHSType, typename RHSType>
class MatrixBinaryOperation;

template <typename T, int Rows, int Cols1, int Cols2>
class MatrixBinaryOperation<Eigen::Matrix<T, Rows, Cols1>,
                            Eigen::Matrix<T, Rows, Cols2>> {
 public:
  typedef Eigen::Matrix<T, Rows, Cols1> LHSType;
  typedef Eigen::Matrix<T, Rows, Cols2> RHSType;
  typedef Eigen::Matrix<T, Rows, SumOrDynamic(Cols1, Cols2)> HStackReturnType;

  static std::enable_if_t<(Rows != Eigen::Dynamic), HStackReturnType> HStack(
      const LHSType& lhs, const RHSType& rhs) {
    HStackReturnType ret;
    ret.resize(Rows, lhs.cols() + rhs.cols());
    ret << lhs, rhs;
    return ret;
  }
};

template <typename T, int Cols, int Rows1, int Rows2>
class MatrixBinaryOperation<Eigen::Matrix<T, Rows1, Cols>,
                            Eigen::Matrix<T, Rows2, Cols>> {
 public:
  typedef Eigen::Matrix<T, Rows1, Cols> LHSType;
  typedef Eigen::Matrix<T, Rows2, Cols> RHSType;
  typedef Eigen::Matrix<T, SumOrDynamic(Rows1, Rows2), Cols> VStackReturnType;

  static std::enable_if_t<(Cols != Eigen::Dynamic), VStackReturnType> VStack(
      const LHSType& lhs, const RHSType& rhs) {
    VStackReturnType ret;
    ret.resize(lhs.rows() + rhs.rows(), Cols);
    ret << lhs, rhs;
    return ret;
  }
};

template <typename T, int Rows, int Cols>
class MatrixBinaryOperation<Eigen::Matrix<T, Rows, Cols>,
                            Eigen::Matrix<T, Rows, Cols>> {
 public:
  typedef Eigen::Matrix<T, Rows, Cols> LHSType;
  typedef Eigen::Matrix<T, Rows, Cols> RHSType;
  typedef Eigen::Matrix<T, Rows, SumOrDynamic(Cols, Cols)> HStackReturnType;
  typedef Eigen::Matrix<T, SumOrDynamic(Rows, Rows), Cols> VStackReturnType;

  static std::enable_if_t<(Rows != Eigen::Dynamic), HStackReturnType> HStack(
      const LHSType& lhs, const RHSType& rhs) {
    HStackReturnType ret;
    ret.resize(Rows, lhs.cols() + rhs.cols());
    ret << lhs, rhs;
    return ret;
  }

  static std::enable_if_t<(Cols != Eigen::Dynamic), VStackReturnType> VStack(
      const LHSType& lhs, const RHSType& rhs) {
    VStackReturnType ret;
    ret.resize(lhs.rows() + rhs.rows(), Cols);
    ret << lhs, rhs;
    return ret;
  }
};

#if defined(HAS_OPENCV)
template <typename T, int Rows, int Cols1, int Cols2>
class MatrixBinaryOperation<cv::Matx<T, Rows, Cols1>,
                            cv::Matx<T, Rows, Cols2>> {
 public:
  typedef cv::Matx<T, Rows, Cols1> LHSType;
  typedef cv::Matx<T, Rows, Cols2> RHSType;
  typedef cv::Matx<T, Rows, Cols1 + Cols2> HStackReturnType;

  static HStackReturnType HStack(const LHSType& lhs, const RHSType& rhs) {
    HStackReturnType ret;
    internal::HStack(lhs, rhs, &ret);
    return ret;
  }
};

template <typename T, int Cols, int Rows1, int Rows2>
class MatrixBinaryOperation<cv::Matx<T, Rows1, Cols>,
                            cv::Matx<T, Rows2, Cols>> {
 public:
  typedef cv::Matx<T, Rows1, Cols> LHSType;
  typedef cv::Matx<T, Rows2, Cols> RHSType;
  typedef cv::Matx<T, Rows1 + Rows2, Cols> VStackReturnType;

  static VStackReturnType VStack(const LHSType& lhs, const RHSType& rhs) {
    VStackReturnType ret;
    internal::VStack(lhs, rhs, &ret);
    return ret;
  }
};

template <typename T, int Rows, int Cols>
class MatrixBinaryOperation<cv::Matx<T, Rows, Cols>, cv::Matx<T, Rows, Cols>> {
 public:
  typedef cv::Matx<T, Rows, Cols> LHSType;
  typedef cv::Matx<T, Rows, Cols> RHSType;
  typedef cv::Matx<T, Rows, Cols + Cols> HStackReturnType;
  typedef cv::Matx<T, Rows + Rows, Cols> VStackReturnType;

  static HStackReturnType HStack(const LHSType& lhs, const RHSType& rhs) {
    HStackReturnType ret;
    internal::HStack(lhs, rhs, &ret);
    return ret;
  }

  static VStackReturnType VStack(const LHSType& lhs, const RHSType& rhs) {
    VStackReturnType ret;
    internal::VStack(lhs, rhs, &ret);
    return ret;
  }
};

template <typename T, int Rows, int Cols>
class MatrixBinaryOperation<cv::Mat_<T>, cv::Matx<T, Rows, Cols>> {
 public:
  typedef cv::Mat_<T> LHSType;
  typedef cv::Matx<T, Rows, Cols> RHSType;
  typedef cv::Mat_<T> HStackReturnType;
  typedef cv::Mat_<T> VStackReturnType;

  static HStackReturnType HStack(const LHSType& lhs, const RHSType& rhs) {
    if (lhs.empty()) return {};
    if (lhs.rows != rhs.rows) return {};
    HStackReturnType ret(lhs.rows, lhs.cols + rhs.cols);
    internal::HStack(lhs, rhs, &ret);
    return ret;
  }

  static VStackReturnType VStack(const LHSType& lhs, const RHSType& rhs) {
    if (lhs.empty()) return {};
    if (lhs.cols != rhs.cols) return {};
    VStackReturnType ret(lhs.rows + rhs.rows, lhs.cols);
    internal::VStack(lhs, rhs, &ret);
    return ret;
  }
};

template <typename T, int Rows, int Cols>
class MatrixBinaryOperation<cv::Matx<T, Rows, Cols>, cv::Mat_<T>> {
 public:
  typedef cv::Matx<T, Rows, Cols> LHSType;
  typedef cv::Mat_<T> RHSType;
  typedef cv::Mat_<T> HStackReturnType;
  typedef cv::Mat_<T> VStackReturnType;

  static HStackReturnType HStack(const LHSType& lhs, const RHSType& rhs) {
    if (rhs.empty()) return {};
    if (lhs.rows != rhs.rows) return {};
    HStackReturnType ret(lhs.rows, lhs.cols + rhs.cols);
    internal::HStack(lhs, rhs, &ret);
    return ret;
  }

  static VStackReturnType VStack(const LHSType& lhs, const RHSType& rhs) {
    if (rhs.empty()) return {};
    if (lhs.cols != rhs.cols) return {};
    VStackReturnType ret(lhs.rows + rhs.rows, lhs.cols);
    internal::VStack(lhs, rhs, &ret);
    return ret;
  }
};

template <typename T>
class MatrixBinaryOperation<cv::Mat_<T>, cv::Mat_<T>> {
 public:
  typedef cv::Mat_<T> LHSType;
  typedef cv::Mat_<T> RHSType;
  typedef cv::Mat_<T> HStackReturnType;
  typedef cv::Mat_<T> VStackReturnType;

  static HStackReturnType HStack(const LHSType& lhs, const RHSType& rhs) {
    if (lhs.empty() || rhs.empty()) return {};
    if (lhs.rows != rhs.rows) return {};
    HStackReturnType ret(lhs.rows, lhs.cols + rhs.cols);
    internal::HStack(lhs, rhs, &ret);
    return ret;
  }

  static VStackReturnType VStack(const LHSType& lhs, const RHSType& rhs) {
    if (lhs.empty() || rhs.empty()) return {};
    if (lhs.cols != rhs.cols) return {};
    VStackReturnType ret(lhs.rows + rhs.rows, lhs.cols);
    internal::VStack(lhs, rhs, &ret);
    return ret;
  }
};
#endif

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_MATH_MATRIX_UTIL_H_