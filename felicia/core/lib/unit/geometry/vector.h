#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_VECTOR_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_VECTOR_H_

#include "Eigen/Core"

#include "felicia/core/lib/unit/unit_helper.h"

namespace felicia {

template <typename T>
class Vector {
 public:
  typedef ::Eigen::Matrix<T, 2, 1> EigenMatrixType;

  constexpr Vector() : x_{0}, y_{0} {}
  constexpr Vector(T x, T y) : x_(x), y_(y) {}
  constexpr Vector(const Vector& other) = default;
  Vector& operator=(const Vector& other) = default;

  void set_xy(T x, T y) {
    x_ = x;
    y_ = y;
  }
  void set_x(T x) { x_ = x; }
  void set_y(T y) { y_ = y; }

  constexpr T x() const { return x_; }
  constexpr T y() const { return y_; }

  double Norm() const { return std::sqrt(SquaredNorm()); }
  double SquaredNorm() const {
    return static_cast<double>(x_) * x_ + static_cast<double>(y_) * y_;
  }

  Vector Scale(T s) const { return Vector{x_ * s, y_ * s}; }
  Vector& ScaleInPlace(T s) {
    x_ *= s;
    y_ *= s;
    return *this;
  }

  Vector Normalize() const {
    return Scale(::base::ClampDiv(1.0, SquaredNorm()));
  }
  Vector& NormalizeInplace() {
    ScaleInPlace(::base::ClampDiv(1.0, SquaredNorm()));
    return *this;
  }

  EigenMatrixType ToEigenVector() const {
    EigenMatrixType vec;
    vec << x_, y_;
    return vec;
  }

 private:
  T x_;
  T y_;
};

template <typename T>
inline bool operator==(const Vector<T>& lhs, const Vector<T>& rhs) {
  return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

template <typename T>
inline bool operator!=(const Vector<T>& lhs, const Vector<T>& rhs) {
  return !(lhs == rhs);
}

typedef Vector<float> Vectorf;
typedef Vector<double> Vectord;

template <typename T>
class Vector3 {
 public:
  typedef ::Eigen::Matrix<T, 3, 1> EigenMatrixType;

  constexpr Vector3() : x_(0), y_(0), z_(0) {}
  constexpr Vector3(T x, T y, T z) : x_(x), y_(y), z_(z) {}
  constexpr explicit Vector3(const Vector<T>& point)
      : x_(point.x_), y_(point.y_), z_(0) {}
  constexpr Vector3(const Vector3& other) = default;
  Vector3& operator=(const Vector3& other) = default;

  void set_xyz(T x, T y, T z) {
    x_ = x;
    y_ = y;
    z_ = z;
  }
  void set_x(T x) { x_ = x; }
  void set_y(T y) { y_ = y; }
  void set_z(T z) { z_ = z; }

  constexpr T x() const { return x_; }
  constexpr T y() const { return y_; }
  constexpr T z() const { return z_; }

  double Norm() const { return std::sqrt(SquaredNorm()); }
  double SquaredNorm() const {
    return static_cast<double>(x_) * x_ + static_cast<double>(y_) * y_ +
           static_cast<double>(z_) * z_;
  }

  Vector3 Scale(T s) const { return Vector3{x_ * s, y_ * s, z_ * s}; }
  Vector3& ScaleInPlace(T s) {
    x_ *= s;
    y_ *= s;
    z_ *= s;
    return *this;
  }

  Vector3 Normalize() const {
    return Scale(::base::ClampDiv(1.0, SquaredNorm()));
  }
  Vector3& NormalizeInplace() {
    ScaleInPlace(::base::ClampDiv(1.0, SquaredNorm()));
    return *this;
  }

  Vector3 operator+(const Vector3& other) const {
    T x = ::base::ClampAdd(x_, other.x_);
    T y = ::base::ClampAdd(y_, other.y_);
    T z = ::base::ClampAdd(z_, other.z_);
    return Vector3{x, y, z};
  }
  Vector3& operator+=(const Vector3& other) {
    x_ = ::base::ClampAdd(x_, other.x_);
    y_ = ::base::ClampAdd(y_, other.y_);
    z_ = ::base::ClampAdd(z_, other.z_);
    return *this;
  }
  Vector3 operator-(const Vector3& other) const {
    T x = ::base::ClampSub(x_, other.x_);
    T y = ::base::ClampSub(y_, other.y_);
    T z = ::base::ClampSub(z_, other.z_);
    return Vector3{x, y, z};
  }
  Vector3& operator-=(const Vector3& other) {
    x_ = ::base::ClampSub(x_, other.x_);
    y_ = ::base::ClampSub(y_, other.y_);
    z_ = ::base::ClampSub(z_, other.z_);
    return *this;
  }

  EigenMatrixType ToEigenVector() const {
    EigenMatrixType vec;
    vec << x, y, z;
    return vec;
  }

 private:
  T x_;
  T y_;
  T z_;
};

template <typename T>
inline bool operator==(const Vector3<T>& lhs, const Vector3<T>& rhs) {
  return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
}

template <typename T>
inline bool operator!=(const Vector3<T>& lhs, const Vector3<T>& rhs) {
  return !(lhs == rhs);
}

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_VECTOR_H_