#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_TRANSFORM_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_TRANSFORM_H_

#include "Eigen/Core"
#include "Eigen/Geometry"

#include "felicia/core/lib/unit/geometry/quaternion.h"
#include "felicia/core/lib/unit/geometry/vector.h"

namespace felicia {

template <typename T>
class Transform {
 public:
  typedef ::Eigen::Transform<T, 2, ::Eigen::Affine> EigenTransformType;

  constexpr Transform()
      : transform_(EigenTransformType::MatrixType::Identity()) {}
  constexpr explicit Transform(const EigenTransformType& transform)
      : transform_(transform) {}
  constexpr Transform(const Transform& other) = default;
  Transform& operator=(const Transform& other) = default;

  Transform inverse() const { return Transform{transform_.inverse()}; }

  void set_transform(const EigenTransformType& transform) {
    transform_ = transform;
  }

  EigenTransformType& ToEigenTransform() { return transform_; }
  const EigenTransformType& ToEigenTransform() const { return transform_; }

  Transform& AddTranslate(T x, T y) {
    transform_ = ::Eigen::Translation<T, 2>(x, y) * transform_;
    return *this;
  }

  // |angle| should be in radian.
  Transform& AddRotation(T angle) {
    transform_ = ::Eigen::Rotation2D<T>(angle) * transform_;
    return *this;
  }

  Transform& AddScale(T x, T y) {
    transform_ = ::Eigen::Scaling(x, y) * transform_;
    return *this;
  }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF_VECTORIZABLE_FIXED_SIZE(
      EigenTransformType::Scalar,
      EigenTransformType::Dim == ::Eigen::Dynamic
          ? ::Eigen::Dynamic
          : (EigenTransformType::Dim + 1) * (EigenTransformType::Dim + 1))

 private:
  EigenTransformType transform_;
};

template <typename T>
inline bool operator==(const Transform<T>& lhs, const Transform<T>& rhs) {
  return lhs.ToEigenTransform() == rhs.ToEigenTransform();
}

template <typename T>
inline bool operator!=(const Transform<T>& lhs, const Transform<T>& rhs) {
  return !(lhs == rhs);
}

typedef Transform<float> Transformf;
typedef Transform<double> Transformd;

template <typename T>
class Transform3 {
 public:
  typedef ::Eigen::Transform<T, 3, ::Eigen::Affine> EigenTransformType;

  constexpr Transform3()
      : transform_(EigenTransformType::MatrixType::Identity()) {}
  constexpr explicit Transform3(const EigenTransformType& transform)
      : transform_(transform) {}
  constexpr Transform3(const Transform3& other) = default;
  Transform3& operator=(const Transform3& other) = default;

  Transform3 inverse() const { return Transform3{transform_.inverse()}; }

  void set_transform(const EigenTransformType& transform) {
    transform_ = transform;
  }

  EigenTransformType& ToEigenTransform() { return transform_; }
  const EigenTransformType& ToEigenTransform() const { return transform_; }

  Transform3& AddTranslate(T x, T y, T z) {
    transform_ = ::Eigen::Translation<T, 3>(x, y, z) * transform_;
    return *this;
  }

  // |angle| should be in radian.
  Transform3& AddRotation(T angle, const Vector3<T>& axis) {
    transform_ =
        ::Eigen::AngleAxis<T>(angle, axis.ToEigenVector()) * transform_;
    return *this;
  }

  Transform3& AddRotation(const Quaternion<T>& quaternion) {
    transform_ = quaternion.ToEigenQuaternion() * transform_;
    return *this;
  }

  Transform3& AddScale(T x, T y, T z) {
    transform_ = ::Eigen::Scaling(x, y, z) * transform_;
    return *this;
  }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF_VECTORIZABLE_FIXED_SIZE(
      EigenTransformType::Scalar,
      EigenTransformType::Dim == ::Eigen::Dynamic
          ? ::Eigen::Dynamic
          : (EigenTransformType::Dim + 1) * (EigenTransformType::Dim + 1))

 private:
  EigenTransformType transform_;
};

template <typename T>
inline bool operator==(const Transform3<T>& lhs, const Transform3<T>& rhs) {
  return lhs.ToEigenTransform() == rhs.ToEigenTransform();
}

template <typename T>
inline bool operator!=(const Transform3<T>& lhs, const Transform3<T>& rhs) {
  return !(lhs == rhs);
}

typedef Transform3<float> Transform3f;
typedef Transform3<double> Transform3d;

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_TRANSFORM_H_