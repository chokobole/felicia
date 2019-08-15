#ifndef FELICIA_SLAM_CAMERA_EPIPOLAR_GEOMETRY_H_
#define FELICIA_SLAM_CAMERA_EPIPOLAR_GEOMETRY_H_

#include "felicia/core/lib/math/matrix_util.h"
#include "felicia/core/lib/unit/geometry/native_matrix_reference.h"
#include "felicia/core/lib/unit/geometry/rigid_body_transform.h"

namespace felicia {
namespace slam {

class EpipolarGeometry {
 public:
  // Compute R[t]x, where [t]x is the matrix representation
  // of the cross product  with t.
  // MatrixType should be any kind of 3x3 Matrix and
  // VectorType should be any kind of 3x1 Matrix.
  template <typename MatrixType, typename VectorType>
  static MatrixType ComputeEssentialMatrix(const MatrixType& R,
                                           const VectorType& t);
  template <typename RigidBodyTransform3Type>
  static auto ComputeEssentialMatrix(const RigidBodyTransform3Type& T);

  // Compute K2.transpose.inverse * E * K.inverse
  // MatrixType should be any kind of 3x3 Matrix and
  // VectorType should be any kind of 3x1 Matrix.
  template <typename MatrixType, typename VectorType>
  static MatrixType ComputeFundamentalMatrix(const MatrixType& K,
                                             const MatrixType& K2,
                                             const MatrixType& R,
                                             const VectorType& t);
  template <typename MatrixType, typename RigidBodyTransform3Type>
  static MatrixType ComputeFundamentalMatrix(const MatrixType& K,
                                             const MatrixType& K2,
                                             const RigidBodyTransform3Type& T);

#if defined(HAS_OPENCV)
  // Compute projection matrix from the essential matrix |E|.
  // |image_points| and |image_points2| are used to determined to relational
  // positions among camera and image_points.
  // MatrixType should be any kind of 3x3 Matrix and
  // PointsType should be any kind of 2xn Matrix.
  template <typename RigidBodyTransform3Type, typename MatrixType,
            typename PointsType>
  static RigidBodyTransform3Type ComputeRigidBodyTransform(
      const MatrixType& E, const PointsType& image_points,
      const PointsType& image_points2);
#endif
};

// static
template <typename MatrixType, typename VectorType>
MatrixType EpipolarGeometry::ComputeEssentialMatrix(const MatrixType& R,
                                                    const VectorType& t) {
  typedef NativeMatrixRef<MatrixType> MatrixTypeRef;
  typedef ConstNativeMatrixRef<VectorType> ConstVectorTypeRef;
  typedef typename ConstVectorTypeRef::ScalarType ScalarType;

  MatrixType mat = MatrixTypeRef::Zero();
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

  return mat * R;
}

// static
template <typename RigidBodyTransform3Type>
auto EpipolarGeometry::ComputeEssentialMatrix(
    const RigidBodyTransform3Type& T) {
  return EpipolarGeometry::ComputeEssentialMatrix(T.rotation_matrix(),
                                                  T.translation_vector());
}

// static
template <typename MatrixType, typename VectorType>
MatrixType EpipolarGeometry::ComputeFundamentalMatrix(const MatrixType& K,
                                                      const MatrixType& K2,
                                                      const MatrixType& R,
                                                      const VectorType& t) {
  typedef ConstNativeMatrixRef<MatrixType> ConstMatrixTypeRef;
  ConstMatrixTypeRef K_ref(K);
  ConstMatrixTypeRef K2_ref(K2);
  MatrixType E = ComputeEssentialMatrix(R, t);

  return K2_ref.transpose().inverse() * E * K_ref.inverse();
}

// static
template <typename MatrixType, typename RigidBodyTransform3Type>
MatrixType EpipolarGeometry::ComputeFundamentalMatrix(
    const MatrixType& K, const MatrixType& K2,
    const RigidBodyTransform3Type& T) {
  return EpipolarGeometry::ComputeFundamentalMatrix(K, K2, T.rotation_matrix(),
                                                    T.translation_vector());
}

#if defined(HAS_OPENCV)
// This was taken and modified from
// https://github.com/introlab/rtabmap/blob/4b118ae6600dbda769eb6993223eda30d3d90b1d/corelib/src/EpipolarGeometry.cpp#L132-L173
template <typename PointsType, typename MatrixType, typename TrnaslationType>
int InFrontOfBothCameras(const PointsType& image_points,
                         const PointsType& image_points2, const MatrixType& R,
                         const TrnaslationType& T) {
  cv::Matx34d p;
  p(0, 0) = 1;
  p(1, 1) = 1;
  p(2, 2) = 1;
  cv::Matx34d p2;
  internal::HStack(R, T, &p2);

  cv::Mat1d points;
  cv::triangulatePoints(p, p2, image_points, image_points2, points);

  ConstNativeMatrixRef<PointsType> image_points_ref(image_points);
  // http://en.wikipedia.org/wiki/Essential_matrix#3D_points_from_corresponding_image_points
  int valid_num = 0;
  for (int i = 0; i < image_points_ref.cols(); ++i) {
    // the five to ignore when all points are super close to the camera
    if (points(2, i) / points(3, i) > 5) {
      ++valid_num;
    }
  }

  return valid_num;
}

// This was taken and modified from
// https://github.com/introlab/rtabmap/blob/4b118ae6600dbda769eb6993223eda30d3d90b1d/corelib/src/EpipolarGeometry.cpp#L175-L290
// static
template <typename RigidBodyTransform3Type, typename MatrixType,
          typename PointsType>
RigidBodyTransform3Type EpipolarGeometry::ComputeRigidBodyTransform(
    const MatrixType& E, const PointsType& image_points,
    const PointsType& image_points2) {
  typedef NativeMatrixRef<MatrixType> MatrixTypeRef;
  typedef ConstNativeMatrixRef<MatrixType> ConstMatrixTypeRef;
  static_assert(bool(ConstMatrixTypeRef::CvMatrix),
                "Not implemetned besides cv matrix.");

  // Refer to
  // https://en.wikipedia.org/wiki/Essential_matrix#Determining_R_and_t_from_E
  MatrixType W = MatrixTypeRef::zeros(3, 3);
  MatrixTypeRef W_ref(W);
  W_ref.at(0, 1) = -1;
  W_ref.at(1, 0) = 1;
  W_ref.at(2, 2) = 1;

  cv::InputArray array;

  MatrixType e = E;
  cv::SVD svd(e);
  cv::Mat u = svd.u;
  cv::Mat vt = svd.vt;
  cv::Mat s = svd.w;

  // E = u*diag(1,1,0)*vt
  MatrixType diag = MatrixTypeRef::Identity(3, 3);
  diag.at(2, 2) = 0;
  e = u * diag * vt;
  svd(e);
  u = svd.u;
  vt = svd.vt;
  s = svd.w;

  MatrixType r = u * W * vt;
  ConstMatrixTypeRef r_ref(r);
  if (r_ref.determinant() + 1.0 < 1e-9) {
    // according to
    // http://en.wikipedia.org/wiki/Essential_matrix#Showing_that_it_is_valid
    e = -E;
    svd(e);
    u = svd.u;
    vt = svd.vt;
    s = svd.w;
  }
  MatrixType Wt = W.t();

  // INFO: There 4 cases of P, only one have all the points in
  // front of the two cameras (positive z).

  cv::Mat R1 = u * W * vt;
  cv::Mat R2 = u * Wt * vt;

  cv::Mat t1 = u.col(2);
  cv::Mat t2 = u.col(2) * -1;

  RigidBodyTransform3Type possible_solutions[] = {
      {R1, t1}, {R1, t2}, {R2, t1}, {R2, t2}};
  RigidBodyTransform3Type answer;
  int max = -1;
  for (auto& solution : possible_solutions) {
    int n = InFrontOfBothCameras(solution.rotation_matrix(),
                                 solution.translation_vector());
    if (n > max) {
      max = n;
      answer = solution;
    }
  }

  return answer;
}
#endif

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_CAMERA_EPIPOLAR_GEOMETRY_H_