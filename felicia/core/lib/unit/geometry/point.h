#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_POINT_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_POINT_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/protobuf/geometry.pb.h"

namespace felicia {

template <typename T>
class Point {
 public:
  constexpr Point() = default;
  constexpr Point(float x, float y) : x_(x), y_(y) {}
  constexpr Point(const Point& other) = default;
  Point& operator=(const Point& other) = default;

  void Scale(float s) {
    x_ = s * x_;
    y_ = s * y_;
  }

  void set_xy(float x, float y) {
    x_ = x;
    y_ = y;
  }
  void set_x(float x) { x_ = x; }
  void set_y(float y) { y_ = y; }

  constexpr float x() const { return x_; }
  constexpr float y() const { return y_; }

 private:
  float x_;
  float y_;
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
  constexpr Point3(float x, float y, float z) : x_(x), y_(y), z_(z) {}
  constexpr explicit Point3(const Point<T>& point)
      : x_(point.x()), y_(point.y()), z_(0) {}
  constexpr Point3(const Point3& other) = default;
  Point3& operator=(const Point3& other) = default;

  void Scale(float s) {
    x_ = s * x_;
    y_ = s * y_;
    z_ = s * z_;
  }

  void set_xyz(float x, float y, float z) {
    x_ = x;
    y_ = y;
    z_ = z;
  }
  void set_x(float x) { x_ = x; }
  void set_y(float y) { y_ = y; }
  void set_z(float z) { z_ = z; }

  constexpr float x() const { return x_; }
  constexpr float y() const { return y_; }
  constexpr float z() const { return z_; }

 private:
  float x_;
  float y_;
  float z_;
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

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_POINT_H_