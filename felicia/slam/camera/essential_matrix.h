#ifndef FELICIA_SLAM_CAMERA_ESSENTIAL_MATRIX_H_
#define FELICIA_SLAM_CAMERA_ESSENTIAL_MATRIX_H_

#include "third_party/chromium/base/logging.h"

#include "felicia/core/lib/math/matrix_util.h"
#include "felicia/core/lib/unit/geometry/native_matrix_reference.h"
#include "felicia/core/lib/unit/geometry/rigid_body_transform.h"

namespace felicia {
namespace slam {

template <typename MatrixType_>
class EssentialMatrix {
 public:
  typedef MatrixType_ MatrixType;
  typedef NativeMatrixRef<MatrixType> MatrixTypeRef;
  typedef ConstNativeMatrixRef<MatrixType> ConstMatrixTypeRef;
  typedef typename ConstMatrixTypeRef::ScalarType ScalarType;
  static_assert(IsMatrix<3, 3>(ConstMatrixTypeRef::Rows,
                               ConstMatrixTypeRef::Cols) ||
                    IsDynamicMatrix(ConstMatrixTypeRef::Rows,
                                    ConstMatrixTypeRef::Cols),
                FEL_MATRIX_SIZE_NOT_SATISFIED("EssentialMatrix", "3x3"));

  EssentialMatrix() = default;
  explicit EssentialMatrix(const MatrixType& matrix) : matrix_(matrix) {
    ConstMatrixTypeRef matrix_ref(matrix_);
    CHECK((matrix_ref.empty() ||
           IsMatrix<3, 3>(matrix_ref.rows(), matrix_ref.cols())));
  }

  // Compute R[t]x, where [t]x is the matrix representation
  // of the cross product  with t.
  // MatrixType should be any kind of 3x3 Matrix and
  // VectorType should be any kind of 3x1 Matrix.
  template <typename VectorType>
  static EssentialMatrix ComputeFrom(const MatrixType& R, const VectorType& t) {
    typedef ConstNativeMatrixRef<VectorType> ConstVectorTypeRef;
    MatrixType mat = MatrixTypeRef::Zero(3, 3);
    MatrixTypeRef mat_ref(mat);
    ConstVectorTypeRef t_ref(t);
    ScalarType x = t_ref.at(0, 0);
    ScalarType y = t_ref.at(1, 0);
    ScalarType z = t_ref.at(2, 0);

    mat_ref.at(0, 1) = -z;
    mat_ref.at(0, 2) = y;
    mat_ref.at(1, 0) = z;
    mat_ref.at(1, 2) = -x;
    mat_ref.at(2, 0) = -y;
    mat_ref.at(2, 1) = x;

    return EssentialMatrix{mat * R};
  }

  template <typename RigidBodyTransform3Type>
  static EssentialMatrix ComputeFrom(const RigidBodyTransform3Type& T) {
    return ComputeFrom(T.rotation_matrix(), T.translation_vector());
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

#endif  // FELICIA_SLAM_CAMERA_ESSENTIAL_MATRIX_H_