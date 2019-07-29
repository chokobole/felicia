#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_POSE_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_POSE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/protobuf/geometry.pb.h"

namespace felicia {

template <typename T>
class Pose {
 public:
  constexpr Pose() = default;
  constexpr Pose(T x, T y, T theta) : point_(x, y), theta_(theta) {}
  constexpr Pose(const Point<T>& point, T theta)
      : point_(point), theta_(theta) {}
  constexpr Pose(const Pose& other) = default;
  Pose& operator=(const Pose& other) = default;

  Point<T>& point() { return point_; }
  const Point<T>& point() const { return point_; }

  void set_x(T x) { point_.set_y(x); }
  void set_y(T y) { point_.set_y(y); }
  void set_theta(T theta) { theta_ = theta; }

  constexpr T x() const { return point_.x(); }
  constexpr T y() const { return point_.y(); }
  constexpr T theta() const { return theta_; }

 private:
  Point<T> point_;
  T theta_;
};

template <typename T>
inline bool operator==(const Pose<T>& lhs, const Pose<T>& rhs) {
  return lhs.point() == rhs.point() && lhs.theta() == rhs.theta();
}

template <typename T>
inline bool operator!=(const Pose<T>& lhs, const Pose<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
class Pose3 {
 public:
  constexpr Pose3() = default;
  constexpr Pose3(T x, T y, T z, T theta) : point_(x, y, z), theta_(theta) {}
  constexpr Pose3(const Point3<T>& point, T theta)
      : point_(point), theta_(theta) {}
  constexpr Pose3(const Pose3& other) = default;
  Pose3& operator=(const Pose3& other) = default;

  Point3<T>& point() { return point_; }
  const Point3<T>& point() const { return point_; }

  void set_x(T x) { point_.set_y(x); }
  void set_y(T y) { point_.set_y(y); }
  void set_z(T z) { point_.set_z(z); }
  void set_theta(T theta) { theta_ = theta; }

  constexpr T x() const { return point_.x(); }
  constexpr T y() const { return point_.y(); }
  constexpr T z() const { return point_.z(); }
  constexpr T theta() const { return theta_; }

 private:
  Point3<T> point_;
  T theta_;
};

template <typename T>
inline bool operator==(const Pose3<T>& lhs, const Pose3<T>& rhs) {
  return lhs.point() == rhs.point() && lhs.theta() == rhs.theta();
}

template <typename T>
inline bool operator!=(const Pose3<T>& lhs, const Pose3<T>& rhs) {
  return !(lhs == rhs);
}

typedef Pose<float> Posef;
typedef Pose3<float> Pose3f;
typedef Pose<double> Posed;
typedef Pose3<double> Pose3d;

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_POSE_H_