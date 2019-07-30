#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_POINT_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_POINT_H_

#include "Eigen/Geometry"

#include "third_party/chromium/base/numerics/clamped_math.h"

#include "felicia/core/lib/unit/geometry/transform.h"
#include "felicia/core/lib/unit/geometry/vector.h"

namespace felicia {

template <typename T>
class Point {
 public:
  constexpr Point() : x_(0), y_(0) {}
  constexpr Point(T x, T y) : x_(x), y_(y) {}
  constexpr Point(const Point& other) = default;
  Point& operator=(const Point& other) = default;

  void set_xy(T x, T y) {
    x_ = x;
    y_ = y;
  }
  void set_x(T x) { x_ = x; }
  void set_y(T y) { y_ = y; }

  constexpr T x() const { return x_; }
  constexpr T y() const { return y_; }

  double Distance(const Point& other) const {
    Vector<T> vec = operator-(other);
    return vec.Norm();
  }

  Point Translate(const Vector<T>& vec) const {
    T x = ::base::ClampAdd(x_, vec.x());
    T y = ::base::ClampAdd(y_, vec.y());
    return Point{x, y};
  }
  Point& TranslateInPlace(const Vector<T>& vec) {
    x_ = ::base::ClampAdd(x_, vec.x());
    y_ = ::base::ClampAdd(y_, vec.y());
    return *this;
  }

  Point Scale(T s) const { return Point{x_ * s, y_ * s}; }
  Point& ScaleInPlace(T s) {
    x_ *= s;
    y_ *= s;
    return *this;
  }

  Point Transform(const Transform<T>& transform) const {
    ::Eigen::Matrix<T, 2, 1> vec;
    vec << x_, y_;
    ::Eigen::Matrix<T, 2, 1> transformed = transform.transform() * vec;
    return Point{transformed[0], transformed[1]};
  }

  Point operator+(const Vector<T>& vec) const { return Translate(vec); }
  Point& operator+=(const Vector<T>& vec) { return TranslateInPlace(vec); }
  Vector<T> operator-(const Point& other) const {
    T x = ::base::ClampSub(x_, other.x_);
    T y = ::base::ClampSub(y_, other.y_);
    return Vector<T>{x, y};
  }
  Point operator*(T s) const { return Scale(s); }
  Point& operator*=(T s) { return ScaleInPlace(s); }

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

template <typename T, typename U>
inline Point<T> operator*(U a, const Point<T>& point) {
  return point * a;
}

typedef Point<float> Pointf;
typedef Point<double> Pointd;

template <typename T>
class Point3 {
 public:
  constexpr Point3() : x_(0), y_(0), z_(0) {}
  constexpr Point3(T x, T y, T z) : x_(x), y_(y), z_(z) {}
  constexpr explicit Point3(const Point<T>& point)
      : x_(point.x()), y_(point.y()), z_(0) {}
  constexpr Point3(const Point3& other) = default;
  Point3& operator=(const Point3& other) = default;

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

  double Distance(const Point3& other) const {
    Vector3<T> vector = operator-(other);
    return vector.Norm();
  }

  Point3 Translate(const Vector3<T>& vector) const {
    T x = ::base::ClampAdd(x_, vector.x());
    T y = ::base::ClampAdd(y_, vector.y());
    T z = ::base::ClampAdd(z_, vector.z());
    return Point3{x, y, z};
  }
  Point3& TranslateInPlace(const Vector3<T>& vector) {
    x_ = ::base::ClampAdd(x_, vector.x());
    y_ = ::base::ClampAdd(y_, vector.y());
    z_ = ::base::ClampAdd(z_, vector.z());
    return *this;
  }

  Point3 Scale(T s) const { return Point3{x_ * s, y_ * s, z_ * s}; }
  Point3& ScaleInPlace(T s) {
    x_ *= s;
    y_ *= s;
    z_ *= s;
    return *this;
  }

  Point3 operator+(const Vector3<T>& vector) const { return Translate(vector); }
  Point3& operator+=(const Vector3<T>& vector) {
    return TranslateInPlace(vector);
  }
  Vector3<T> operator-(const Point3& other) const {
    T x = ::base::ClampSub(x_, other.x_);
    T y = ::base::ClampSub(y_, other.y_);
    T z = ::base::ClampSub(z_, other.z_);
    return Vector3<T>{x, y, z};
  }
  Point3 operator*(T s) const { return Scale(s); }
  Point3& operator*=(T s) { return ScaleInPlace(s); }

  Point3 Transform(const Transform3<T>& transform) const {
    ::Eigen::Matrix<T, 3, 1> vec;
    vec << x_, y_, z_;
    ::Eigen::Matrix<T, 3, 1> transformed = transform.transform() * vec;
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

template <typename T, typename U>
inline Point3<T> operator*(U a, const Point3<T>& point) {
  return point * a;
}

typedef Point3<float> Point3f;
typedef Point3<double> Point3d;

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_POINT_H_