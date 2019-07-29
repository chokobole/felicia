#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_POINT_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_POINT_H_

#include "Eigen/Geometry"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/protobuf/geometry.pb.h"

namespace felicia {

template <typename T>
class Point {
 public:
  constexpr Point() = default;
  constexpr Point(T x, T y) : x_(x), y_(y) {}
  constexpr Point(const Point& other) = default;
  Point& operator=(const Point& other) = default;

  void Scale(T s) {
    x_ = s * x_;
    y_ = s * y_;
  }

  T distance(const Point& other) {
    T dx = (x_ - other.x_);
    T dy = (y_ - other.y_);
    return std::sqrt(dx * dx + dy * dy);
  }

  void set_xy(T x, T y) {
    x_ = x;
    y_ = y;
  }
  void set_x(T x) { x_ = x; }
  void set_y(T y) { y_ = y; }

  constexpr T x() const { return x_; }
  constexpr T y() const { return y_; }

  Point Transform(
      const ::Eigen::Transform<T, 2, ::Eigen::Affine>& transform) const {
    ::Eigen::Matrix<T, 2, 1> vec;
    vec << x_, y_;
    ::Eigen::Matrix<T, 2, 1> transformed = transform * vec;
    return Point{transformed[0], transformed[1]};
  }

 private:
  T x_;
  T y_;
};

template <typename T>
inline bool operator==(const Point<T>& lhs, const Point<T>& rhs) {
  return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

template <typename T>
inline bool operator!=(const Point<T>& lhs, const Point<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
class Point3 {
 public:
  constexpr Point3() = default;
  constexpr Point3(T x, T y, T z) : x_(x), y_(y), z_(z) {}
  constexpr explicit Point3(const Point<T>& point)
      : x_(point.x()), y_(point.y()), z_(0) {}
  constexpr Point3(const Point3& other) = default;
  Point3& operator=(const Point3& other) = default;

  void Scale(T s) {
    x_ = s * x_;
    y_ = s * y_;
    z_ = s * z_;
  }

  T distance(const Point3& other) {
    T dx = (x_ - other.x_);
    T dy = (y_ - other.y_);
    T dz = (z_ - other.z_);
    return std::sqrt(dx * dx + dy * dy + dz * dz);
  }

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

  Point3 Transform(
      const ::Eigen::Transform<T, 3, ::Eigen::Affine>& transform) const {
    ::Eigen::Matrix<T, 3, 1> vec;
    vec << x_, y_, z_;
    ::Eigen::Matrix<T, 3, 1> transformed = transform * vec;
    return Point3{transformed[0], transformed[1], transformed[2]};
  }

 private:
  T x_;
  T y_;
  T z_;
};

template <typename T>
inline bool operator==(const Point3<T>& lhs, const Point3<T>& rhs) {
  return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
}

template <typename T>
inline bool operator!=(const Point3<T>& lhs, const Point3<T>& rhs) {
  return !(lhs == rhs);
}

typedef Point<float> Pointf;
typedef Point3<float> Point3f;
typedef Point<double> Pointd;
typedef Point3<double> Point3d;

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_POINT_H_