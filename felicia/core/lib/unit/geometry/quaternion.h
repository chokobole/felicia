#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_QUATERNION_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_QUATERNION_H_

#include "Eigen/Core"
#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/vector.h"
#include "felicia/core/protobuf/geometry.pb.h"

namespace felicia {

namespace {

const double kEpsilon = 1e-5;

}  // namespace

template <typename T>
class Quaternion {
 public:
  typedef Eigen::Quaternion<T> EigenQuaternionType;

  constexpr Quaternion() : x_(0), y_(0), z_(0), w_(1) {}
  constexpr Quaternion(T x, T y, T z, T w) : x_(x), y_(y), z_(z), w_(w) {}
  constexpr Quaternion(const Vector3<T>& axis, T angle) {
    Vector3<T> normalized = axis.Normalize();
    angle *= 0.5;
    double s = sin(angle);
    x_ = normalized.x() * s;
    y_ = normalized.y() * s;
    z_ = normalized.z() * s;
    w_ = cos(angle);
  }
  constexpr Quaternion(const Quaternion& other) = default;
  Quaternion& operator=(const Quaternion& other) = default;

  void set_xyzw(T x, T y, T z, T w) {
    x_ = x;
    y_ = y;
    z_ = z;
    w_ = w;
  }
  void set_x(T x) { x_ = x; }
  void set_y(T y) { y_ = y; }
  void set_z(T z) { z_ = z; }
  void set_w(T w) { w_ = w; }

  constexpr T x() const { return x_; }
  constexpr T y() const { return y_; }
  constexpr T z() const { return z_; }
  constexpr T w() const { return w_; }

  constexpr bool IsValid() const {
    return std::isfinite(x_) && std::isfinite(y_) && std::isfinite(z_) &&
           std::isfinite(w_);
  }

  void set_vector(const Vector3<T>& vector) {
    x_ = vector.x();
    y_ = vector.y();
    z_ = vector.z();
  }
  constexpr Vector3<T> vector() const { return {x_, y_, z_}; }

  double Norm() const { return std::sqrt(SquaredNorm()); }
  double SquaredNorm() const {
    return static_cast<double>(x_) * x_ + static_cast<double>(y_) * y_ +
           static_cast<double>(z_) * z_ + static_cast<double>(w_) * w_;
  }

  constexpr Quaternion inverse() const { return Quaternion{-x_, -y_, -z_, w_}; }

  Quaternion Scale(T s) const {
    return Quaternion{x_ * s, y_ * s, z_ * s, w_ * s};
  }
  Quaternion& ScaleInPlace(T s) {
    x_ *= s;
    y_ *= s;
    z_ *= s;
    w_ *= s;
    return *this;
  }

  Quaternion Normalize() const { return Scale(base::ClampDiv(1.0, Norm())); }
  Quaternion& NormalizeInPlace() {
    ScaleInPlace(base::ClampDiv(1.0, Norm()));
    return *this;
  }

  // This was taken and modified from
  // https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/ui/gfx/geometry/quaternion.cc#L59-L78
  Quaternion Slerp(const Quaternion& q, double t) const {
    double dot = x_ * q.x_ + y_ * q.y_ + z_ * q.z_ + w_ * q.w_;

    // Clamp dot to -1.0 <= dot <= 1.0.
    dot = std::min(std::max(dot, -1.0), 1.0);

    // Quaternions are facing the same direction.
    if (std::abs(dot - 1.0) < kEpsilon || std::abs(dot + 1.0) < kEpsilon)
      return *this;

    double denom = std::sqrt(1.0 - dot * dot);
    double theta = std::acos(dot);
    double w = std::sin(t * theta) * (1.0 / denom);

    double s1 = std::cos(t * theta) - dot * w;
    double s2 = w;

    return (s1 * *this) + (s2 * q);
  }

  Quaternion Lerp(const Quaternion& q, double t) const {
    return ((1.0 - t) * *this) + t * q;
  }

  Quaternion operator+(const Quaternion& other) const {
    return {x_ + other.x_, y_ + other.y_, z_ + other.z_, w_ + other.w_};
  }
  Quaternion& operator+=(const Quaternion& other) {
    x_ += other.x_;
    y_ += other.y_;
    z_ += other.z_;
    w_ += other.w_;
    return *this;
  }

  Quaternion operator-(const Quaternion& other) const {
    return {x_ - other.x_, y_ - other.y_, z_ - other.z_, w_ - other.w_};
  }
  Quaternion& operator-=(const Quaternion& other) {
    x_ -= other.x_;
    y_ -= other.y_;
    z_ -= other.z_;
    w_ -= other.w_;
    return *this;
  }

  Quaternion operator*(const Quaternion& other) const {
    T x = w_ * other.x_ + x_ * other.w_ + y_ * other.z_ - z_ * other.y_;
    T y = w_ * other.y_ - x_ * other.z_ + y_ * other.w_ + z_ * other.x_;
    T z = w_ * other.z_ + x_ * other.y_ - y_ * other.x_ + z_ * other.w_;
    T w = w_ * other.w_ - x_ * other.x_ - y_ * other.y_ - z_ * other.z_;
    return {x, y, z, w};
  }
  Quaternion& operator*=(const Quaternion& other) {
    T x = w_ * other.x_ + x_ * other.w_ + y_ * other.z_ - z_ * other.y_;
    T y = w_ * other.y_ - x_ * other.z_ + y_ * other.w_ + z_ * other.x_;
    T z = w_ * other.z_ + x_ * other.y_ - y_ * other.x_ + z_ * other.w_;
    T w = w_ * other.w_ - x_ * other.x_ - y_ * other.y_ - z_ * other.z_;
    x_ = x;
    y_ = y;
    z_ = z;
    w_ = w;
    return *this;
  }

  Quaternion operator*(T a) const {
    T x = x_ * a;
    T y = y_ * a;
    T z = z_ * a;
    T w = w_ * a;
    return {x, y, z, w};
  }
  Quaternion& operator*=(T a) {
    x_ *= a;
    y_ *= a;
    z_ *= a;
    w_ *= a;
    return *this;
  }

  std::string ToString() const {
    return base::StringPrintf(
        "(%s, %s, %s, %s)", base::NumberToString(x_).c_str(),
        base::NumberToString(y_).c_str(), base::NumberToString(z_).c_str(),
        base::NumberToString(w_).c_str());
  }

  EigenQuaternionType ToEigenQuaternion() const { return {w_, x_, y_, z_}; }

 private:
  T x_;
  T y_;
  T z_;
  T w_;
};

template <typename T>
inline bool operator==(const Quaternion<T>& lhs, const Quaternion<T>& rhs) {
  return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z() &&
         lhs.w() == rhs.w();
}

template <typename T>
inline bool operator!=(const Quaternion<T>& lhs, const Quaternion<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Quaternion<T>& quaternion) {
  return os << quaternion.ToString();
}

template <typename T, typename U>
inline Quaternion<T> operator*(U a, const Quaternion<T>& quaternion) {
  return quaternion * a;
}

typedef Quaternion<float> Quaternionf;
typedef Quaternion<double> Quaterniond;

template <typename MessageType, typename T>
MessageType QuaternionToQuaternionMessage(const Quaternion<T>& quaternion) {
  MessageType message;
  message.set_w(quaternion.w());
  message.set_x(quaternion.x());
  message.set_y(quaternion.y());
  message.set_z(quaternion.z());
  return message;
}

EXPORT QuaternionfMessage
QuaternionfToQuaternionfMessage(const Quaternionf& quaternion);
EXPORT QuaterniondMessage
QuaterniondToQuaterniondMessage(const Quaterniond& quaternion);

template <typename T, typename MessageType>
Quaternion<T> QuaternionMessageToQuaternion(const MessageType& message) {
  return {message.x(), message.y(), message.z(), message.w()};
}

EXPORT Quaternionf
QuaternionfMessageToQuaternionf(const QuaternionfMessage& message);
EXPORT Quaterniond
QuaterniondMessageToQuaterniond(const QuaterniondMessage& message);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_QUATERNION_H_