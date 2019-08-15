#ifndef FELICIA_SLAM_CAMERA_CAMERA_MATRIX_H_
#define FELICIA_SLAM_CAMERA_CAMERA_MATRIX_H_

#include "third_party/chromium/base/logging.h"

#include "felicia/core/lib/math/matrix_util.h"
#include "felicia/core/lib/unit/geometry/native_matrix_reference.h"
#include "felicia/core/lib/unit/geometry/point.h"

namespace felicia {
namespace slam {

template <typename MatrixType>
class CameraMatrix {
 public:
  typedef NativeMatrixRef<MatrixType> MatrixTypeRef;
  typedef ConstNativeMatrixRef<MatrixType> ConstMatrixTypeRef;
  typedef typename ConstMatrixTypeRef::ScalarType ScalarType;
  static_assert(IsMatrix<3, 3>(ConstMatrixTypeRef::Rows,
                               ConstMatrixTypeRef::Cols) ||
                    IsDynamicMatrix(ConstMatrixTypeRef::Rows,
                                    ConstMatrixTypeRef::Cols),
                FEL_MATRIX_SIZE_NOT_SATISFIED("CameraMatrix", "3x3"));

  CameraMatrix() = default;
  explicit CameraMatrix(const MatrixType& matrix) : matrix_(matrix) {
    ConstMatrixTypeRef matrix_ref(matrix_);
    CHECK((matrix_ref.empty() ||
           IsMatrix<3, 3>(matrix_ref.rows(), matrix_ref.cols())));
  }
  CameraMatrix(ScalarType fx, ScalarType fy, ScalarType cx, ScalarType cy)
      : matrix_(ConstMatrixTypeRef::Zero(3, 3)) {
    set_fx(fx);
    set_fy(fy);
    set_cx(cx);
    set_cy(cx);
  }

  void operator=(const MatrixType& matrix) {
    ConstMatrixTypeRef matrix_ref(matrix);
    CHECK((matrix_ref.empty() ||
           IsMatrix<3, 3>(matrix_ref.rows(), matrix_ref.cols())));
    matrix_ = matrix;
  }

  ScalarType fx() const { return ValueOrZero(0, 0); }
  void set_fx(ScalarType fx) { MaybeSet(0, 0, fx); }

  ScalarType fy() const { return ValueOrZero(1, 1); }
  void set_fy(ScalarType fy) { Maybeset(1, 1, fy); }

  ScalarType cx() const { return ValueOrZero(0, 2); }
  void set_cx(ScalarType cx) { MaybeSet(0, 2, cx); }

  ScalarType cy() const { return ValueOrZero(1, 2); }
  void set_cy(ScalarType cy) { MaybeSet(1, 2, cy); }

  const MatrixType& matrix() const { return matrix_; }
  MatrixType& matrix() { return matrix_; }

  void set_matrix(const MatrixType& matrix) { matrix_ = matrix; }

  template <typename T>
  Point<T> Project(const Point3<T>& point) const {
    T x = point.x();
    T y = point.y();
    T z = point.z();
    CHECK_NE(z, 0.f);
    double inv_z = 1.0 / z;
    T u = fx() * x * inv_z + cx();
    T v = fy() * y * inv_z + cy();
    return {u, v};
  }

  template <typename T>
  Point3<T> ProjectInverse(const Point<T> point, T depth) const {
    T u = point.x();
    T v = point.y();
    T x = (u - cx()) * depth / fx();
    T y = (v - cy()) * depth / fy();
    T z = depth;
    return {x, y, z};
  }

 private:
  ScalarType ValueOrZero(int row, int col) const {
    ConstMatrixTypeRef matrix_ref(matrix_);
    if (matrix_ref.empty()) return 0;
    if (matrix_ref.rows() <= row || matrix_ref.cols() <= col) return 0;
    return matrix_ref.at(row, col);
  }

  void MaybeSet(int row, int col, ScalarType value) {
    MatrixTypeRef matrix_ref(matrix_);
    if (matrix_ref.empty()) return;
    if (matrix_ref.rows() <= row || matrix_ref.cols() <= col) return;
    matrix_ref.at(row, col) = value;
  }

  MatrixType matrix_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_CAMERA_CAMERA_MATRIX_H_