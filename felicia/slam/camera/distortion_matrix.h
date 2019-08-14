#ifndef FELICIA_SLAM_CAMERA_DISTORTION_MATRIX_H_
#define FELICIA_SLAM_CAMERA_DISTORTION_MATRIX_H_

#include "felicia/core/lib/unit/geometry/native_matrix_reference.h"

namespace felicia {
namespace slam {

constexpr bool IsValidDistortionMatrix(int rows, int cols) {
  return rows == 1 && (cols == 4 || cols == 5 || cols == 6 || cols == 8);
}

template <typename MatrixType>
class DistortionMatrix {
 public:
  typedef NativeMatrixRef<MatrixType> MatrixTypeRef;
  typedef ConstNativeMatrixRef<MatrixType> ConstMatrixTypeRef;
  typedef typename ConstMatrixTypeRef::ScalarType ScalarType;
  static_assert(
      IsValidDistortionMatrix(ConstMatrixTypeRef::Rows,
                              ConstMatrixTypeRef::Cols) ||
          (ConstMatrixTypeRef::Rows == 1 && ConstMatrixTypeRef::Cols == -1) ||
          (ConstMatrixTypeRef::Rows == -1 && ConstMatrixTypeRef::Cols == -1),
      "DistortionMatrix should be 1xn matrix, otherwise it should be "
      "possibly changed to that form.");

  DistortionMatrix() = default;
  explicit DistortionMatrix(const MatrixType& matrix) : matrix_(matrix) {
    ConstMatrixTypeRef matrix_ref(matrix_);
    CHECK((matrix_ref.empty() ||
           IsValidDistortionMatrix(matrix_ref.rows(), matrix_ref.cols())));
  }
  DistortionMatrix(ScalarType k1, ScalarType k2, ScalarType p1, ScalarType p2)
      : matrix_(ConstMatrixTypeRef::Zero(1, 4)) {
    set_k1(k1);
    set_k2(k2);
    set_p1(p1);
    set_p2(p2);
  }
  DistortionMatrix(ScalarType k1, ScalarType k2, ScalarType p1, ScalarType p2,
                   ScalarType k3)
      : matrix_(ConstMatrixTypeRef::Zero(1, 5)) {
    set_k1(k1);
    set_k2(k2);
    set_p1(p1);
    set_p2(p2);
    set_k3(k3);
  }
  DistortionMatrix(ScalarType k1, ScalarType k2, ScalarType p1, ScalarType p2,
                   ScalarType k3, ScalarType k4)
      : matrix_(ConstMatrixTypeRef::Zero(1, 6)) {
    set_k1(k1);
    set_k2(k2);
    set_p1(p1);
    set_p2(p2);
    set_k3(k3);
    set_k4(k4);
  }
  DistortionMatrix(ScalarType k1, ScalarType k2, ScalarType p1, ScalarType p2,
                   ScalarType k3, ScalarType k4, ScalarType k5, ScalarType k6)
      : matrix_(ConstMatrixTypeRef::Zero(1, 8)) {
    set_k1(k1);
    set_k2(k2);
    set_p1(p1);
    set_p2(p2);
    set_k3(k3);
    set_k4(k4);
    set_k5(k5);
    set_k6(k6);
  }

  void operator=(const MatrixType& matrix) {
    ConstMatrixTypeRef matrix_ref(matrix);
    CHECK((matrix_ref.empty() ||
           IsValidDistortionMatrix(matrix_ref.rows(), matrix_ref.cols())));
    matrix_ = matrix;
  }

  ScalarType k1() const { return ValueOrZero(0); }
  void set_k1(ScalarType k1) { MaybeSet(0, k1); }

  ScalarType k2() const { return ValueOrZero(1); }
  void set_k2(ScalarType k2) { MaybeSet(1, k2); }

  ScalarType p1() const { return ValueOrZero(2); }
  void set_p1(ScalarType p1) { MaybeSet(2, k2); }

  ScalarType p2() const { return ValueOrZero(3); }
  void set_p2(ScalarType p2) { MaybeSet(3, p2); }

  ScalarType k3() const { return ValueOrZero(4); }
  void set_k3(ScalarType k3) { MaybeSet(4, k3); }

  ScalarType k4() const { return ValueOrZero(5); }
  void set_k4(ScalarType k4) { MaybeSet(5, k4); }

  ScalarType k5() const { return ValueOrZero(6); }
  void set_k5(ScalarType k5) { MaybeSet(6, k5); }

  ScalarType k6() const { return ValueOrZero(7); }
  void set_k6(ScalarType k6) { MaybeSet(7, k6); }

  const MatrixType& matrix() const { return matrix_; }
  MatrixType& matrix() { return matrix_; }

  void set_matrix(const MatrixType& matrix) { matrix_ = matrix; }

 private:
  ScalarType ValueOrZero(int index) const {
    ConstMatrixTypeRef matrix_ref(matrix_);
    if (matrix_ref.empty()) return 0;
    if (matrix_ref.cols() <= index) return 0;
    return matrix_ref.at(0, index);
  }

  void MaybeSet(int index, ScalarType value) {
    MatrixTypeRef matrix_ref(matrix_);
    if (matrix_ref.empty()) return;
    if (matrix_ref.cols() <= index) return;
    matrix_ref.at(0, index) = value;
  }

  MatrixType matrix_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_CAMERA_DISTORTION_MATRIX_H_