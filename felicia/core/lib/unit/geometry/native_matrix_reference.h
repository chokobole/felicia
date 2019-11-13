// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_NATIVE_MATRIX_REFERENCE_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_NATIVE_MATRIX_REFERENCE_H_

#include <type_traits>

#include "Eigen/Core"

#if defined(HAS_OPENCV)
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>
#endif

namespace felicia {

// NOTE: This is a reference type, it doesn't copy the real matrix inside.
// Its constructor looks like below.
// explicit NativeMatrixRef(SomeMatrixType& matrix);
template <typename T>
class NativeMatrixRef;

// NOTE: This is a const reference type, it doesn't copy the real matrix inside.
// Its constructor looks like below.
// explicit ConstNativeMatrixRef(const SomeMatrixType& matrix);
template <typename T>
class ConstNativeMatrixRef;

template <typename T, int Rows_, int Cols_, int Options_>
class NativeMatrixRef<Eigen::Matrix<T, Rows_, Cols_, Options_>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Matrix<T, Rows_, Cols_, Options_> MatrixType;

  enum {
    EigenMatrix = 1,
    Rows = Rows_,
    Cols = Cols_,
    Options = Options_,
  };

  explicit NativeMatrixRef(MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero(int rows = Rows, int cols = Cols) {
    return MatrixType::Zero(rows, cols);
  }
  static MatrixType Identity(int rows = Rows, int cols = Cols) {
    return MatrixType::Identity(rows, cols);
  }
  static MatrixType NewMatrix(ScalarType* data, int rows = Rows,
                              int cols = Cols) {
    return Eigen::Map<MatrixType>(data, rows, cols);
  }

  bool empty() const {
    if (Rows_ != Eigen::Dynamic && Cols_ != Eigen::Dynamic) return false;
    return matrix_.rows() * matrix_.cols() == 0;
  }

  MatrixType transpose() const { return matrix_.transpose(); }

  MatrixType inverse() const { return matrix_.inverse(); }

  double determinant() const { return matrix_.determinant(); }

  auto block(int start_row, int start_col, int rows, int cols) const {
    return matrix_.block(start_row, start_col, rows, cols);
  }

  ScalarType& at(int row, int col) { return matrix_(row, col); }
  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  int rows() const { return matrix_.rows(); }
  int cols() const { return matrix_.cols(); }

  size_t size() const { return matrix_.size(); }
  size_t allocation_size() const { return matrix_.size() * sizeof(ScalarType); }

  ScalarType* data() { return matrix_.data(); }
  const ScalarType* data() const { return matrix_.data(); }

#if defined(HAS_OPENCV)
  void ToCvMatrix(cv::Mat* mat) const { cv::eigen2cv(matrix_, *mat); }

  void ToCvMatrix(cv::Mat_<T>* mat) const { cv::eigen2cv(matrix_, *mat); }

  void ToCvMatrix(cv::Matx<T, Rows_, Cols_>* mat) const {
    cv::eigen2cv(matrix_, *mat);
  }
#endif

 private:
  MatrixType& matrix_;
};

typedef NativeMatrixRef<Eigen::Matrix2f> EigenMatrix2fRef;
typedef NativeMatrixRef<Eigen::Matrix2d> EigenMatrix2dRef;
typedef NativeMatrixRef<Eigen::Matrix3f> EigenMatrix3fRef;
typedef NativeMatrixRef<Eigen::Matrix3d> EigenMatrix3dRef;
typedef NativeMatrixRef<Eigen::Vector2f> EigenVector2fRef;
typedef NativeMatrixRef<Eigen::Vector2d> EigenVector2dRef;
typedef NativeMatrixRef<Eigen::Vector3f> EigenVector3fRef;
typedef NativeMatrixRef<Eigen::Vector3d> EigenVector3dRef;

template <typename T, int Rows_, int Cols_, int Options_>
class ConstNativeMatrixRef<Eigen::Matrix<T, Rows_, Cols_, Options_>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Matrix<T, Rows_, Cols_, Options_> MatrixType;

  enum {
    EigenMatrix = 1,
    Rows = Rows_,
    Cols = Cols_,
    Options = Options_,
  };

  explicit ConstNativeMatrixRef(const MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero(int rows = Rows, int cols = Cols) {
    return MatrixType::Zero(rows, cols);
  }
  static MatrixType Identity(int rows = Rows, int cols = Cols) {
    return MatrixType::Identity(rows, cols);
  }
  static MatrixType NewMatrix(ScalarType* data, int rows = Rows,
                              int cols = Cols) {
    return Eigen::Map<MatrixType>(data, rows, cols);
  }

  bool empty() const {
    if (Rows_ != Eigen::Dynamic && Cols_ != Eigen::Dynamic) return false;
    return matrix_.rows() * matrix_.cols() == 0;
  }

  MatrixType clone() const { return matrix_; }

  MatrixType transpose() const { return matrix_.transpose(); }

  MatrixType inverse() const { return matrix_.inverse(); }

  double determinant() const { return matrix_.determinant(); }

  auto block(int start_row, int start_col, int rows, int cols) const {
    return matrix_.block(start_row, start_col, rows, cols);
  }

  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  int rows() const { return matrix_.rows(); }
  int cols() const { return matrix_.cols(); }

  size_t size() const { return matrix_.size(); }
  size_t allocation_size() const { return matrix_.size() * sizeof(ScalarType); }

  const ScalarType* data() const { return matrix_.data(); }

#if defined(HAS_OPENCV)
  void ToCvMatrix(cv::Mat* mat) const { cv::eigen2cv(matrix_, *mat); }

  void ToCvMatrix(cv::Mat_<T>* mat) const { cv::eigen2cv(matrix_, *mat); }

  void ToCvMatrix(cv::Matx<T, Rows_, Cols_>* mat) const {
    cv::eigen2cv(matrix_, *mat);
  }
#endif

 private:
  const MatrixType& matrix_;
};

typedef ConstNativeMatrixRef<Eigen::Matrix2f> ConstEigenMatrix2fRef;
typedef ConstNativeMatrixRef<Eigen::Matrix2d> ConstEigenMatrix2dRef;
typedef ConstNativeMatrixRef<Eigen::Matrix3f> ConstEigenMatrix3fRef;
typedef ConstNativeMatrixRef<Eigen::Matrix3d> ConstEigenMatrix3dRef;
typedef ConstNativeMatrixRef<Eigen::Vector2f> ConstEigenVector2fRef;
typedef ConstNativeMatrixRef<Eigen::Vector2d> ConstEigenVector2dRef;
typedef ConstNativeMatrixRef<Eigen::Vector3f> ConstEigenVector3fRef;
typedef ConstNativeMatrixRef<Eigen::Vector3d> ConstEigenVector3dRef;

#if defined(HAS_OPENCV)
template <>
class NativeMatrixRef<cv::Mat> {
 public:
  // To conform to other NativeMatrixRef's
  typedef double ScalarType;
  typedef cv::Mat MatrixType;

  enum {
    CvMatrix = 1,
    Rows = -1,
    Cols = -1,
  };

  explicit NativeMatrixRef(MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero(int rows, int cols, int type = CV_64FC1) {
    return MatrixType::zeros(rows, cols, type);
  }
  static MatrixType Identity(int rows, int cols, int type = CV_64FC1) {
    return MatrixType::eye(rows, cols, type);
  }
  static MatrixType NewMatrix(void* data, int rows, int cols,
                              int type = CV_64FC1,
                              size_t step = cv::Mat::AUTO_STEP) {
    return MatrixType(rows, cols, type, data, step).clone();
  }

  bool empty() const { return matrix_.empty(); }

  MatrixType clone() const { return matrix_; }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  double determinant() const { return cv::determinant(matrix_); }

  // To conform to other NativeMatrixRef's
  ScalarType& at(int row, int col) {
    return matrix_.template at<ScalarType>(row, col);
  }
  const ScalarType& at(int row, int col) const {
    return matrix_.template at<ScalarType>(row, col);
  }

  template <typename T,
            std::enable_if_t<!std::is_same<T, double>::value>* = nullptr>
  T& at(int row, int col) {
    return matrix_.template at<T>(row, col);
  }
  template <typename T,
            std::enable_if_t<!std::is_same<T, double>::value>* = nullptr>
  const T& at(int row, int col) const {
    return matrix_.template at<T>(row, col);
  }

  int rows() const { return matrix_.rows; }
  int cols() const { return matrix_.cols; }

  size_t size() const { return matrix_.total(); }
  size_t allocation_size() const {
    return matrix_.total() * matrix_.elemSize();
  }

  const void* data() const { return matrix_.data; }
  void* data() { return matrix_.data; }

  template <typename Derived>
  void ToEigenMatrix(Eigen::MatrixBase<Derived>* matrix) const {
    cv::cv2eigen(matrix_, *matrix);
  }

 private:
  MatrixType& matrix_;
};

typedef NativeMatrixRef<cv::Mat> CvMatRef;

template <>
class ConstNativeMatrixRef<cv::Mat> {
 public:
  // To conform to other ConstNativeMatrixRef's
  typedef double ScalarType;
  typedef cv::Mat MatrixType;

  enum {
    CvMatrix = 1,
    Rows = -1,
    Cols = -1,
  };

  explicit ConstNativeMatrixRef(const MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero(int rows, int cols, int type = CV_64FC1) {
    return MatrixType::zeros(rows, cols, type);
  }
  static MatrixType Identity(int rows, int cols, int type = CV_64FC1) {
    return MatrixType::eye(rows, cols, type);
  }
  static MatrixType NewMatrix(void* data, int rows, int cols,
                              int type = CV_64FC1,
                              size_t step = cv::Mat::AUTO_STEP) {
    return MatrixType(rows, cols, type, data, step).clone();
  }

  bool empty() const { return matrix_.empty(); }

  MatrixType clone() const { return matrix_.clone(); }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  double determinant() const { return cv::determinant(matrix_); }

  MatrixType block(int start_row, int start_col, int rows, int cols) const {
    return {matrix_, cv::Range(start_row, rows - start_row),
            cv::Range(start_col, cols - start_col)};
  }

  // To conform to other ConstNativeMatrixRef's
  const ScalarType& at(int row, int col) const {
    return matrix_.template at<ScalarType>(row, col);
  }

  template <typename T,
            std::enable_if_t<!std::is_same<T, double>::value>* = nullptr>
  const T& at(int row, int col) const {
    return matrix_.template at<T>(row, col);
  }

  int rows() const { return matrix_.rows; }
  int cols() const { return matrix_.cols; }

  size_t size() const { return matrix_.total(); }
  size_t allocation_size() const {
    return matrix_.total() * matrix_.elemSize();
  }

  const void* data() const { return matrix_.data; }

  template <typename Derived>
  void ToEigenMatrix(Eigen::MatrixBase<Derived>* matrix) const {
    cv::cv2eigen(matrix_, *matrix);
  }

 private:
  const MatrixType& matrix_;
};

typedef ConstNativeMatrixRef<cv::Mat> ConstCvMatRef;

template <typename T>
class NativeMatrixRef<cv::Mat_<T>> {
 public:
  typedef T ScalarType;
  typedef cv::Mat_<T> MatrixType;

  enum {
    CvMatrix = 1,
    Rows = -1,
    Cols = -1,
  };

  explicit NativeMatrixRef(MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero(int rows, int cols, int type = cv::DataType<T>::type) {
    return MatrixType::zeros(rows, cols);
  }
  static MatrixType Identity(int rows, int cols,
                             int type = cv::DataType<T>::type) {
    return MatrixType::eye(rows, cols);
  }
  static MatrixType NewMatrix(ScalarType* data, int rows, int cols,
                              int type = cv::DataType<T>::type,
                              size_t step = cv::Mat::AUTO_STEP) {
    return MatrixType(rows, cols, type, data, step).clone();
  }

  bool empty() const { return matrix_.empty(); }

  MatrixType clone() const { return matrix_.clone(); }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  double determinant() const { return cv::determinant(matrix_); }

  MatrixType block(int start_row, int start_col, int rows, int cols) const {
    return {matrix_, cv::Range(start_row, rows - start_row),
            cv::Range(start_col, cols - start_col)};
  }

  ScalarType& at(int row, int col) { return matrix_(row, col); }
  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  int rows() const { return matrix_.rows; }
  int cols() const { return matrix_.cols; }

  size_t size() const { return matrix_.total(); }
  size_t allocation_size() const {
    return matrix_.total() * matrix_.elemSize();
  }

  const ScalarType* data() const { return matrix_.data; }
  ScalarType* data() { return matrix_.data; }

  template <typename Derived>
  void ToEigenMatrix(Eigen::MatrixBase<Derived>* matrix) const {
    cv::cv2eigen(matrix_, *matrix);
  }

 private:
  MatrixType& matrix_;
};

typedef NativeMatrixRef<cv::Mat1f> CvMat1fRef;
typedef NativeMatrixRef<cv::Mat1d> CvMat1dRef;

template <typename T>
class ConstNativeMatrixRef<cv::Mat_<T>> {
 public:
  typedef T ScalarType;
  typedef cv::Mat_<T> MatrixType;

  enum {
    CvMatrix = 1,
    Rows = -1,
    Cols = -1,
  };

  explicit ConstNativeMatrixRef(const MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero(int rows, int cols, int type = cv::DataType<T>::type) {
    return MatrixType::zeros(rows, cols);
  }
  static MatrixType Identity(int rows, int cols,
                             int type = cv::DataType<T>::type) {
    return MatrixType::eye(rows, cols);
  }
  static MatrixType NewMatrix(ScalarType* data, int rows, int cols,
                              int type = cv::DataType<T>::type,
                              size_t step = cv::Mat::AUTO_STEP) {
    return MatrixType(rows, cols, type, data, step).clone();
  }

  bool empty() const { return matrix_.empty(); }

  MatrixType clone() const { return matrix_.clone(); }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  double determinant() const { return cv::determinant(matrix_); }

  MatrixType block(int start_row, int start_col, int rows, int cols) const {
    return {matrix_, cv::Range(start_row, rows - start_row),
            cv::Range(start_col, cols - start_col)};
  }

  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  int rows() const { return matrix_.rows; }
  int cols() const { return matrix_.cols; }

  size_t size() const { return matrix_.total(); }
  size_t allocation_size() const {
    return matrix_.total() * matrix_.elemSize();
  }

  const ScalarType* data() const { return matrix_.data; }

  template <typename Derived>
  void ToEigenMatrix(Eigen::MatrixBase<Derived>* matrix) const {
    cv::cv2eigen(matrix_, *matrix);
  }

 private:
  const MatrixType& matrix_;
};

typedef ConstNativeMatrixRef<cv::Mat1f> ConstCvMat1fRef;
typedef ConstNativeMatrixRef<cv::Mat1d> ConstCvMat1dRef;

template <typename T, int Rows_, int Cols_>
class NativeMatrixRef<cv::Matx<T, Rows_, Cols_>> {
 public:
  typedef T ScalarType;
  typedef cv::Matx<T, Rows_, Cols_> MatrixType;

  enum {
    CvMatrix = 1,
    Rows = Rows_,
    Cols = Cols_,
  };

  explicit NativeMatrixRef(MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero(int rows = Rows, int cols = Cols,
                         int type = cv::DataType<T>::type) {
    return MatrixType::zeros();
  }
  static MatrixType Identity(int rows = Rows, int cols = Cols,
                             int type = cv::DataType<T>::type) {
    return MatrixType::eye();
  }
  static MatrixType NewMatrix(ScalarType* data, int rows = Rows,
                              int cols = Cols) {
    return MatrixType(data);
  }

  bool empty() const { return false; }

  MatrixType clone() const { return matrix_; }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  double determinant() const { return cv::determinant(matrix_); }

  ScalarType& at(int row, int col) { return matrix_(row, col); }
  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  constexpr int rows() const { return Rows; }
  constexpr int cols() const { return Cols; }

  constexpr size_t size() const { return Rows * Cols; }
  constexpr size_t allocation_size() const {
    return Rows * Cols * sizeof(ScalarType);
  }

  const ScalarType* data() const { return matrix_.val; }
  ScalarType* data() { return matrix_.val; }

  void ToEigenMatrix(Eigen::Matrix<T, Rows_, Cols_>* matrix) const {
    cv::cv2eigen(matrix_, *matrix);
  }

 private:
  MatrixType& matrix_;
};

typedef NativeMatrixRef<cv::Matx22f> CvMatx22fRef;
typedef NativeMatrixRef<cv::Matx22d> CvMatx22dRef;
typedef NativeMatrixRef<cv::Matx33f> CvMatx33fRef;
typedef NativeMatrixRef<cv::Matx33d> CvMatx33dRef;
typedef NativeMatrixRef<cv::Matx21f> CvMatx21fRef;
typedef NativeMatrixRef<cv::Matx21d> CvMatx21dRef;
typedef NativeMatrixRef<cv::Matx31f> CvMatx31fRef;
typedef NativeMatrixRef<cv::Matx31d> CvMatx31dRef;

template <typename T, int Rows_, int Cols_>
class ConstNativeMatrixRef<cv::Matx<T, Rows_, Cols_>> {
 public:
  typedef T ScalarType;
  typedef cv::Matx<T, Rows_, Cols_> MatrixType;

  enum {
    CvMatrix = 1,
    Rows = Rows_,
    Cols = Cols_,
  };

  explicit ConstNativeMatrixRef(const MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero(int rows = Rows, int cols = Cols,
                         int type = cv::DataType<T>::type) {
    return MatrixType::zeros();
  }
  static MatrixType Identity(int row = Rows, int cols = Cols,
                             int type = cv::DataType<T>::type) {
    return MatrixType::eye();
  }
  static MatrixType NewMatrix(ScalarType* data, int rows = Rows,
                              int cols = Cols) {
    return MatrixType(data);
  }

  bool empty() const { return false; }

  MatrixType clone() const { return matrix_; }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  double determinant() const { return cv::determinant(matrix_); }

  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  constexpr int rows() const { return Rows; }
  constexpr int cols() const { return Cols; }

  constexpr size_t size() const { return Rows * Cols; }
  constexpr size_t allocation_size() const {
    return Rows * Cols * sizeof(ScalarType);
  }

  const ScalarType* data() const { return matrix_.val; }

  void ToEigenMatrix(Eigen::Matrix<T, Rows_, Cols_>* matrix) const {
    cv::cv2eigen(matrix_, *matrix);
  }

 private:
  const MatrixType& matrix_;
};

typedef ConstNativeMatrixRef<cv::Matx22f> ConstCvMatx22fRef;
typedef ConstNativeMatrixRef<cv::Matx22d> ConstCvMatx22dRef;
typedef ConstNativeMatrixRef<cv::Matx33f> ConstCvMatx33fRef;
typedef ConstNativeMatrixRef<cv::Matx33d> ConstCvMatx33dRef;
typedef ConstNativeMatrixRef<cv::Matx21f> ConstCvMatx21fRef;
typedef ConstNativeMatrixRef<cv::Matx21d> ConstCvMatx21dRef;
typedef ConstNativeMatrixRef<cv::Matx31f> ConstCvMatx31fRef;
typedef ConstNativeMatrixRef<cv::Matx31d> ConstCvMatx31dRef;
#endif

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_NATIVE_MATRIX_REFERENCE_H_
