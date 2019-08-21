#ifndef FELICIA_SLAM_CAMERA_RECTIFICATION_MATRIX_H_
#define FELICIA_SLAM_CAMERA_RECTIFICATION_MATRIX_H_

#include "felicia/core/lib/unit/geometry/native_matrix_reference.h"

namespace felicia {
namespace slam {

template <typename MatrixType>
class RectificationMatrix {
 public:
  typedef ConstNativeMatrixRef<MatrixType> ConstMatrixTypeRef;
  typedef typename ConstMatrixTypeRef::ScalarType ScalarType;
  static_assert(IsMatrix<3, 3>(ConstMatrixTypeRef::Rows,
                               ConstMatrixTypeRef::Cols) ||
                    IsDynamicMatrix(ConstMatrixTypeRef::Rows,
                                    ConstMatrixTypeRef::Cols),
                FEL_MATRIX_SIZE_NOT_SATISFIED("RectificationMatrix", "3x3"));

  RectificationMatrix() = default;
  explicit RectificationMatrix(const MatrixType& matrix) : matrix_(matrix) {
    ConstMatrixTypeRef matrix_ref(matrix_);
    CHECK((matrix_ref.empty() ||
           IsMatrix<3, 3>(matrix_ref.rows(), matrix_ref.cols())));
  }

  void operator=(const MatrixType& matrix) {
    ConstMatrixTypeRef matrix_ref(matrix);
    CHECK((matrix_ref.empty() ||
           IsMatrix<3, 3>(matrix_ref.rows(), matrix_ref.cols())));
    matrix_ = matrix;
  }

  const MatrixType& matrix() const { return matrix_; }
  MatrixType& matrix() { return matrix_; }

  void set_matrix(const MatrixType& matrix) { matrix_ = matrix; }

 private:
  MatrixType matrix_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_CAMERA_RECTIFICATION_MATRIX_H_