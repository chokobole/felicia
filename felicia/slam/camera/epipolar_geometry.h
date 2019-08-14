#ifndef FELICIA_SLAM_CAMERA_EPIPOLAR_GEOMETRY_H_
#define FELICIA_SLAM_CAMERA_EPIPOLAR_GEOMETRY_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/native_matrix_reference.h"
#include "felicia/core/lib/unit/geometry/rigid_body_transform.h"

namespace felicia {
namespace slam {

class EXPORT EpiploarGeometry {
 public:
  // Compute R[t]x, where [t]x is the matrix representation of the cross product
  // with t.
  // MatrixType should be any kind of 3x3 Matrix and
  // VectorType should be any kind of 3x1 Matrix or Vector.
  template <typename MatrixType, typename VectorType>
  static MatrixType ComputeEssentialMatrix(const MatrixType& R,
                                           const VectorType& t);
  template <typename RigidBodyTransform3Type>
  static auto ComputeEssentialMatrix(const RigidBodyTransform3Type& T);

  // Compute K2.transpose.inverse * E * K.inverse
  // MatrixType should be any kind of 3x3 Matrix and
  // VectorType should be any kind of 3x1 Matrix or Vector.
  template <typename MatrixType, typename VectorType>
  static MatrixType ComputeFundamentalMatrix(const MatrixType& K,
                                             const MatrixType& K2,
                                             const MatrixType& R,
                                             const VectorType& t);
  template <typename MatrixType, typename RigidBodyTransform3Type>
  static MatrixType ComputeFundamentalMatrix(const MatrixType& K,
                                             const MatrixType& K2,
                                             const RigidBodyTransform3Type& T);
};

// static
template <typename MatrixType, typename VectorType>
MatrixType EpiploarGeometry::ComputeEssentialMatrix(const MatrixType& R,
                                                    const VectorType& t) {
  typedef typename NativeMatrixRef<MatrixType>::ScalarType ScalarType;

  MatrixType mat = NativeMatrixRef<MatrixType>::Zero();
  NativeMatrixRef<MatrixType> mat_ref(mat);
  ConstNativeMatrixRef<VectorType> t_ref(t);
  ScalarType x = t_ref.at(0, 0);
  ScalarType y = t_ref.at(1, 0);
  ScalarType z = t_ref.at(2, 0);

  mat_ref.at(0, 1) = -z;
  mat_ref.at(0, 2) = y;
  mat_ref.at(1, 0) = z;
  mat_ref.at(1, 2) = -x;
  mat_ref.at(2, 0) = -y;
  mat_ref.at(2, 1) = x;

  return mat * R;
}

// static
template <typename RigidBodyTransform3Type>
auto EpiploarGeometry::ComputeEssentialMatrix(
    const RigidBodyTransform3Type& T) {
  return EpiploarGeometry::ComputeEssentialMatrix(T.rotation_matrix(),
                                                  T.translation_vector());
}

// static
template <typename MatrixType, typename VectorType>
MatrixType EpiploarGeometry::ComputeFundamentalMatrix(const MatrixType& K,
                                                      const MatrixType& K2,
                                                      const MatrixType& R,
                                                      const VectorType& t) {
  ConstNativeMatrixRef<MatrixType> K_ref(K);
  ConstNativeMatrixRef<MatrixType> K2_ref(K2);

  MatrixType E = ComputeEssentialMatrix(R, t);

  return K2_ref.transpose().inverse() * E * K_ref.inverse();
}

// static
template <typename MatrixType, typename RigidBodyTransform3Type>
MatrixType EpiploarGeometry::ComputeFundamentalMatrix(
    const MatrixType& K, const MatrixType& K2,
    const RigidBodyTransform3Type& T) {
  return EpiploarGeometry::ComputeFundamentalMatrix(K, K2, T.rotation_matrix(),
                                                    T.translation_vector());
}

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_CAMERA_EPIPOLAR_GEOMETRY_H_