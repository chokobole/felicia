#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_VECTOR_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_VECTOR_H_

#include "Eigen/Core"
#include "third_party/chromium/base/numerics/clamped_math.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/protobuf/geometry.pb.h"

namespace felicia {

template <typename T>
class Vector {
 public:
  typedef Eigen::Matrix<T, 2, 1> EigenMatrixType;

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

  constexpr bool IsValid() const {
    return std::isfinite(x_) && std::isfinite(y_);
  }

  double Norm() const { return std::sqrt(SquaredNorm()); }
  double SquaredNorm() const {
    return static_cast<double>(x_) * x_ + static_cast<double>(y_) * y_;
  }

  template <typename U>
  Vector Scale(U s) const {
    return {static_cast<T>(x_ * s), static_cast<T>(y_ * s)};
  }
  template <typename U>
  Vector& ScaleInPlace(U s) {
    x_ *= s;
    y_ *= s;
    return *this;
  }

  Vector Normalize() const { return Scale(1.0 / Norm()); }
  Vector& NormalizeInplace() {
    ScaleInPlace(1.0 / Norm());
    return *this;
  }

  Vector operator+(const Vector& other) const {
    T x = base::ClampAdd(x_, other.x_);
    T y = base::ClampAdd(y_, other.y_);
    return {x, y};
  }
  Vector& operator+=(const Vector& other) {
    x_ = base::ClampAdd(x_, other.x_);
    y_ = base::ClampAdd(y_, other.y_);
    return *this;
  }
  Vector operator-(const Vector& other) const {
    T x = base::ClampSub(x_, other.x_);
    T y = base::ClampSub(y_, other.y_);
    return {x, y};
  }
  Vector& operator-=(const Vector& other) {
    x_ = base::ClampSub(x_, other.x_);
    y_ = base::ClampSub(y_, other.y_);
    return *this;
  }
  template <typename U>
  Vector operator*(U a) const {
    return Scale(a);
  }
  template <typename U>
  Vector& operator*=(U a) {
    return ScaleInPlace(a);
  }
  template <typename U>
  Vector operator/(U a) const {
    return Scale(1.0 / a);
  }
  template <typename U>
  Vector& operator/=(U a) const {
    return ScaleInPlace(1.0 / a);
  }

  std::string ToString() const {
    return base::StringPrintf("(%s, %s)", base::NumberToString(x_).c_str(),
                              base::NumberToString(y_).c_str());
  }

  EigenMatrixType ToEigenVector() const {
    EigenMatrixType vec(x_, y_);
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

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& vector) {
  return os << vector.ToString();
}

template <typename T, typename U>
inline Vector<T> operator*(U a, const Vector<T>& vector) {
  return vector * a;
}

typedef Vector<int> Vectori;
typedef Vector<float> Vectorf;
typedef Vector<double> Vectord;

template <typename MessageType, typename T>
MessageType VectorToVectorMessage(const Vector<T>& vector) {
  MessageType message;
  message.set_x(vector.x());
  message.set_y(vector.y());
  return message;
}

EXPORT VectoriMessage VectoriToVectoriMessage(const Vectori& vector);
EXPORT VectorfMessage VectorfToVectorfMessage(const Vectorf& vector);
EXPORT VectordMessage VectordToVectordMessage(const Vectord& vector);

template <typename T, typename MessageType>
Vector<T> VectorMessageToVector(const MessageType& message) {
  return {message.x(), message.y()};
}

EXPORT Vectori VectoriMessageToVectori(const VectoriMessage& message);
EXPORT Vectorf VectorfMessageToVectorf(const VectorfMessage& message);
EXPORT Vectord VectordMessageToVectord(const VectordMessage& message);

template <typename T>
class Vector3 {
 public:
  typedef Eigen::Matrix<T, 3, 1> EigenMatrixType;

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

  constexpr bool IsValid() const {
    return std::isfinite(x_) && std::isfinite(y_) && std::isfinite(z_);
  }

  double Norm() const { return std::sqrt(SquaredNorm()); }
  double SquaredNorm() const {
    return static_cast<double>(x_) * x_ + static_cast<double>(y_) * y_ +
           static_cast<double>(z_) * z_;
  }

  template <typename U>
  Vector3 Scale(U s) const {
    return {static_cast<T>(x_ * s), static_cast<T>(y_ * s),
            static_cast<T>(z_ * s)};
  }
  template <typename U>
  Vector3& ScaleInPlace(U s) {
    x_ *= s;
    y_ *= s;
    z_ *= s;
    return *this;
  }

  Vector3 Normalize() const { return Scale(1.0 / Norm()); }
  Vector3& NormalizeInplace() {
    ScaleInPlace(1.0 / Norm());
    return *this;
  }

  Vector3 operator+(const Vector3& other) const {
    T x = base::ClampAdd(x_, other.x_);
    T y = base::ClampAdd(y_, other.y_);
    T z = base::ClampAdd(z_, other.z_);
    return {x, y, z};
  }
  Vector3& operator+=(const Vector3& other) {
    x_ = base::ClampAdd(x_, other.x_);
    y_ = base::ClampAdd(y_, other.y_);
    z_ = base::ClampAdd(z_, other.z_);
    return *this;
  }
  Vector3 operator-(const Vector3& other) const {
    T x = base::ClampSub(x_, other.x_);
    T y = base::ClampSub(y_, other.y_);
    T z = base::ClampSub(z_, other.z_);
    return {x, y, z};
  }
  Vector3& operator-=(const Vector3& other) {
    x_ = base::ClampSub(x_, other.x_);
    y_ = base::ClampSub(y_, other.y_);
    z_ = base::ClampSub(z_, other.z_);
    return *this;
  }
  template <typename U>
  Vector3 operator*(U a) const {
    return Scale(a);
  }
  template <typename U>
  Vector3& operator*=(U a) {
    return ScaleInPlace(a);
  }
  template <typename U>
  Vector3 operator/(U a) const {
    return Scale(1 / a);
  }
  template <typename U>
  Vector3& operator/=(U a) const {
    return ScaleInPlace(1 / a);
  }

  std::string ToString() const {
    return base::StringPrintf("(%s, %s, %s)", base::NumberToString(x_).c_str(),
                              base::NumberToString(y_).c_str(),
                              base::NumberToString(z_).c_str());
  }

  EigenMatrixType ToEigenVector() const {
    EigenMatrixType vec(x_, y_, z_);
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

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector3<T>& vector) {
  return os << vector.ToString();
}

template <typename T, typename U>
inline Vector3<T> operator*(U a, const Vector3<T>& vector) {
  return vector * a;
}

typedef Vector3<int> Vector3i;
typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;

template <typename MessageType, typename T>
MessageType Vector3ToVector3Message(const Vector3<T>& vector) {
  MessageType message;
  message.set_x(vector.x());
  message.set_y(vector.y());
  message.set_z(vector.z());
  return message;
}

EXPORT Vector3iMessage Vector3iToVector3iMessage(const Vector3i& vector);
EXPORT Vector3fMessage Vector3fToVector3fMessage(const Vector3f& vector);
EXPORT Vector3dMessage Vector3dToVector3dMessage(const Vector3d& vector);

template <typename T, typename MessageType>
Vector3<T> Vector3MessageToVector3(const MessageType& message) {
  return {message.x(), message.y(), message.z()};
}

EXPORT Vector3i Vector3iMessageToVector3i(const Vector3iMessage& message);
EXPORT Vector3f Vector3fMessageToVector3f(const Vector3fMessage& message);
EXPORT Vector3d Vector3dMessageToVector3d(const Vector3dMessage& message);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_VECTOR_H_