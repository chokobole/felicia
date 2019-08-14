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
template <typename T, int Rows_ = -1, int Cols_ = -1>
class NativeMatrixRef;

// NOTE: This is a const reference type, it doesn't copy the real matrix inside.
// Its constructor looks like below.
// explicit ConstNativeMatrixRef(const SomeMatrixType& matrix);
template <typename T, int Rows_ = -1, int Cols_ = -1>
class ConstNativeMatrixRef;

template <typename T, int Rows_, int Cols_>
class NativeMatrixRef<Eigen::Matrix<T, Rows_, Cols_>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Matrix<T, Rows_, Cols_> MatrixType;

  enum {
    Rows = Rows_,
    Cols = Cols_,
  };

  explicit NativeMatrixRef(MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero() { return MatrixType::Zero(); }
  static MatrixType Zero(int rows, int cols) {
    return MatrixType::Zero(rows, cols);
  }
  static MatrixType Identity() { return MatrixType::Identity(); }

  bool empty() const {
    if (Rows_ != Eigen::Dynamic && Cols_ != Eigen::Dynamic) return false;
    return matrix_.rows() * matrix_.cols() == 0;
  }

  MatrixType transpose() const { return matrix_.transpose(); }

  MatrixType inverse() const { return matrix_.inverse(); }

  ScalarType& at(int row, int col) { return matrix_(row, col); }
  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  int rows() const { return matrix_.rows(); }
  int cols() const { return matrix_.cols(); }

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

template <typename T, int Rows_, int Cols_>
class ConstNativeMatrixRef<Eigen::Matrix<T, Rows_, Cols_>> {
 public:
  typedef T ScalarType;
  typedef Eigen::Matrix<T, Rows_, Cols_> MatrixType;

  enum {
    Rows = Rows_,
    Cols = Cols_,
  };

  explicit ConstNativeMatrixRef(const MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero() { return MatrixType::Zero(); }
  static MatrixType Zero(int rows, int cols) {
    return MatrixType::Zero(rows, cols);
  }
  static MatrixType Identity() { return MatrixType::Identity(); }

  bool empty() const {
    if (Rows_ != Eigen::Dynamic && Cols_ != Eigen::Dynamic) return false;
    return matrix_.rows() * matrix_.cols() == 0;
  }

  MatrixType transpose() const { return matrix_.transpose(); }

  MatrixType inverse() const { return matrix_.inverse(); }

  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  int rows() const { return matrix_.rows(); }
  int cols() const { return matrix_.cols(); }

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
    Rows = -1,
    Cols = -1,
  };

  explicit NativeMatrixRef(MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero(int rows, int cols, int type) {
    return MatrixType::zeros(rows, cols, type);
  }
  static MatrixType Identity(int rows, int cols, int type) {
    return MatrixType::eye(rows, cols, type);
  }

  bool empty() const { return matrix_.empty(); }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  // To conform to other NativeMatrixRef's
  ScalarType& at(int row, int col) {
    return matrix_.template at<ScalarType>(row, col);
  }
  const ScalarType& at(int row, int col) const {
    return matrix_.template at<ScalarType>(row, col);
  }

  template <typename T>
  std::enable_if_t<!std::is_same<T, double>::value, T&> at(int row, int col) {
    return matrix_.template at<T>(row, col);
  }
  template <typename T>
  std::enable_if_t<!std::is_same<T, double>::value, const T&> at(
      int row, int col) const {
    return matrix_.template at<T>(row, col);
  }

  int rows() const { return matrix_.rows; }
  int cols() const { return matrix_.cols; }

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
    Rows = -1,
    Cols = -1,
  };

  explicit ConstNativeMatrixRef(const MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero(int rows, int cols, int type) {
    return MatrixType::zeros(rows, cols, type);
  }
  static MatrixType Identity(int rows, int cols, int type) {
    return MatrixType::eye(rows, cols, type);
  }

  bool empty() const { return matrix_.empty(); }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  // To conform to other ConstNativeMatrixRef's
  const ScalarType& at(int row, int col) const {
    return matrix_.template at<ScalarType>(row, col);
  }

  template <typename T>
  std::enable_if_t<!std::is_same<T, double>::value, const T&> at(
      int row, int col) const {
    return matrix_.template at<T>(row, col);
  }

  int rows() const { return matrix_.rows; }
  int cols() const { return matrix_.cols; }

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

  bool empty() const { return matrix_.empty(); }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  ScalarType& at(int row, int col) { return matrix_(row, col); }
  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  int rows() const { return matrix_.rows; }
  int cols() const { return matrix_.cols; }

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

  bool empty() const { return matrix_.empty(); }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  int rows() const { return matrix_.rows; }
  int cols() const { return matrix_.cols; }

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
    Rows = Rows_,
    Cols = Cols_,
  };

  explicit NativeMatrixRef(MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero() { return MatrixType::zeros(); }
  static MatrixType Zero(int rows, int cols, int type = cv::DataType<T>::type) {
    return MatrixType::zeros();
  }
  static MatrixType Identity(int row, int cols,
                             int type = cv::DataType<T>::type) {
    return MatrixType::eye();
  }

  bool empty() const { return false; }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  ScalarType& at(int row, int col) { return matrix_(row, col); }
  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  int rows() const { return matrix_.rows; }
  int cols() const { return matrix_.cols; }

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
    Rows = Rows_,
    Cols = Cols_,
  };

  explicit ConstNativeMatrixRef(const MatrixType& matrix) : matrix_(matrix) {}

  static MatrixType Zero() { return MatrixType::zeros(); }
  static MatrixType Zero(int rows, int cols, int type = cv::DataType<T>::type) {
    return MatrixType::zeros();
  }
  static MatrixType Identity(int row, int cols,
                             int type = cv::DataType<T>::type) {
    return MatrixType::eye();
  }

  bool empty() const { return false; }

  MatrixType transpose() const { return matrix_.t(); }

  MatrixType inverse() const { return matrix_.inv(); }

  const ScalarType& at(int row, int col) const { return matrix_(row, col); }

  int rows() const { return matrix_.rows; }
  int cols() const { return matrix_.cols; }

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
