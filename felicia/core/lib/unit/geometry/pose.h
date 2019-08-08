#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_POSE_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_POSE_H_

#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/geometry/quaternion.h"
#include "felicia/core/lib/unit/geometry/vector.h"
#include "felicia/core/protobuf/geometry.pb.h"

namespace felicia {

template <typename T>
class Pose {
 public:
  constexpr Pose() : theta_(0) {}
  constexpr Pose(T x, T y, T theta) : point_(x, y), theta_(theta) {}
  constexpr Pose(const Point<T>& point, T theta)
      : point_(point), theta_(theta) {}
  constexpr Pose(const Pose& other) = default;
  Pose& operator=(const Pose& other) = default;

  constexpr Point<T>& point() { return point_; }
  constexpr const Point<T>& point() const { return point_; }

  void set_x(T x) { point_.set_x(x); }
  void set_y(T y) { point_.set_y(y); }
  void set_theta(T theta) { theta_ = theta; }

  constexpr T x() const { return point_.x(); }
  constexpr T y() const { return point_.y(); }
  constexpr T theta() const { return theta_; }

  constexpr bool IsValid() const {
    return point_.IsValid() && !std::isnan(theta_);
  }

  double Distance(const Pose& other) const {
    return point_.Distance(other.point_);
  }

  Pose Translate(const Vector<T>& vector) const {
    return {point_.Translate(vector), theta_};
  }
  Pose& TranslateInPlace(const Vector<T>& vector) {
    point_.TranslateInPlace(vector);
    return *this;
  }

  Pose Scale(T s) const { return Pose{point_.Scale(s), theta_}; }
  Pose& ScaleInPlace(T s) {
    point_.ScaleInPlace(s);
    return *this;
  }

  Pose Rotate(T theta) { return Pose{point_, theta_ + theta}; }
  Pose& RotateInPlace(T theta) {
    theta_ += theta;
    return *this;
  }

  Transform<T> ToTransform() const {
    Transform<T> transform;
    transform.AddRotation(theta_).AddTranslate(point_.x(), point_.y());
    return transform;
  }

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

typedef Pose<float> Posef;
typedef Pose<double> Posed;

template <typename PoseMessageType, typename PointMessageType, typename T>
PoseMessageType PoseToPoseMessage(const Pose<T>& pose) {
  PoseMessageType message;
  *message.mutable_point() =
      PointToPointMessage<PointMessageType>(pose.point());
  message.set_theta(pose.theta());
  return message;
}

EXPORT PosefMessage PosefToPosefMessage(const Posef& pose);
EXPORT PosedMessage PosedToPosedMessage(const Posed& pose);

template <typename T, typename MessageType>
Pose<T> PoseMessageToPose(const MessageType& message) {
  return {PointMessageToPoint<T>(message.point()), message.theta()};
}

EXPORT Posef PosefMessageToPosef(const PosefMessage& message);
EXPORT Posed PosedMessageToPosed(const PosedMessage& message);

template <typename PoseWithTimestampMessageType, typename PointMessageType,
          typename T>
PoseWithTimestampMessageType PoseToPoseWithTimestampMessage(
    const Pose<T>& pose, base::TimeDelta timestamp) {
  PoseWithTimestampMessageType message;
  *message.mutable_point() =
      PointToPointMessage<PointMessageType>(pose.point());
  message.set_theta(pose.theta());
  message.set_timestamp(timestamp.InMicroseconds());
  return message;
}

EXPORT PosefWithTimestampMessage
PosefToPosefWithTimestampMessage(const Posef& pose, base::TimeDelta timestamp);
EXPORT PosedWithTimestampMessage
PosedToPosedWithTimestampMessage(const Posed& pose, base::TimeDelta timestamp);

template <typename T, typename MessageType>
Pose<T> PoseWithTimestampMessageToPose(const MessageType& message) {
  return {PointMessageToPoint<T>(message.point()), message.theta()};
}

EXPORT Posef
PosefWithTimestampMessageToPosef(const PosefWithTimestampMessage& message);
EXPORT Posed
PosedWithTimestampMessageToPosed(const PosedWithTimestampMessage& message);

template <typename T>
class Pose3 {
 public:
  constexpr Pose3() = default;
  constexpr Pose3(T x, T y, T z, const Quaternion<T>& orientation)
      : point_(x, y, z), orientation_(orientation) {}
  constexpr Pose3(const Point3<T>& point, const Quaternion<T>& orientation)
      : point_(point), orientation_(orientation) {}
  constexpr Pose3(const Pose3& other) = default;
  Pose3& operator=(const Pose3& other) = default;

  constexpr Point3<T>& point() { return point_; }
  constexpr const Point3<T>& point() const { return point_; }
  constexpr Quaternion<T>& orientation() { return orientation_; }
  constexpr const Quaternion<T>& orientation() const { return orientation_; }

  void set_x(T x) { point_.set_x(x); }
  void set_y(T y) { point_.set_y(y); }
  void set_z(T z) { point_.set_z(z); }
  void set_orientation(T orientation) { orientation_ = orientation; }

  constexpr T x() const { return point_.x(); }
  constexpr T y() const { return point_.y(); }
  constexpr T z() const { return point_.z(); }

  constexpr bool IsValid() const {
    return point_.IsValid() && orientation_.IsValid();
  }

  double Distance(const Pose3& other) const {
    return point_.Distance(other.point_);
  }

  Pose3 Translate(const Vector3<T>& vector) const {
    return {point_.Translate(vector), orientation_};
  }
  Pose3& TranslateInPlace(const Vector3<T>& vector) {
    point_.TranslateInPlace(vector);
    return *this;
  }

  Pose3 Scale(T s) const { return Pose3{point_.Scale(s), orientation_}; }
  Pose3& ScaleInPlace(T s) {
    point_.ScaleInPlace(s);
    return *this;
  }

  Transform3<T> ToTransform(bool rotate_first = true) const {
    Transform3<T> transform;
    if (rotate_first) {
      transform.AddRotation(orientation_)
          .AddTranslate(point_.x(), point_.y(), point_.z());
    } else {
      transform.AddTranslate(point_.x(), point_.y(), point_.z())
          .AddRotation(orientation_);
    }
    return transform;
  }

 private:
  Point3<T> point_;
  Quaternion<T> orientation_;
};

template <typename T>
inline bool operator==(const Pose3<T>& lhs, const Pose3<T>& rhs) {
  return lhs.point() == rhs.point() && lhs.orientation() == rhs.orientation();
}

template <typename T>
inline bool operator!=(const Pose3<T>& lhs, const Pose3<T>& rhs) {
  return !(lhs == rhs);
}

typedef Pose3<float> Pose3f;
typedef Pose3<double> Pose3d;

template <typename Pose3MessageType, typename Point3MessageType,
          typename QuaternionMessageType, typename T>
Pose3MessageType Pose3ToPose3Message(const Pose3<T>& pose) {
  Pose3MessageType message;
  *message.mutable_point() =
      Point3ToPoint3Message<Point3MessageType>(pose.point());
  *message.mutable_orientation() =
      QuaternionToQuaternionMessage<QuaternionMessageType>(pose.orientation());
  return message;
}

EXPORT Pose3fMessage Pose3fToPose3fMessage(const Pose3f& pose);
EXPORT Pose3dMessage Pose3dToPose3dMessage(const Pose3d& pose);

template <typename T, typename MessageType>
Pose3<T> Pose3MessageToPose3(const MessageType& message) {
  return {Point3MessageToPoint3<T>(message.point()),
          QuaternionMessageToQuaternion<T>(message.orientation())};
}

EXPORT Pose3f Pose3fMessageToPose3f(const Pose3fMessage& message);
EXPORT Pose3d Pose3dMessageToPose3d(const Pose3dMessage& message);

template <typename Pose3WithTimestampMessageType, typename Point3MessageType,
          typename QuaternionMessageType, typename T>
Pose3WithTimestampMessageType Pose3ToPose3WithTimestampMessage(
    const Pose3<T>& pose, base::TimeDelta timestamp) {
  Pose3WithTimestampMessageType message;
  *message.mutable_point() =
      Point3ToPoint3Message<Point3MessageType>(pose.point());
  *message.mutable_orientation() =
      QuaternionToQuaternionMessage<QuaternionMessageType>(pose.orientation());
  message.set_timestamp(timestamp.InMicroseconds());
  return message;
}

EXPORT Pose3fWithTimestampMessage Pose3fToPose3fWithTimestampMessage(
    const Pose3f& pose, base::TimeDelta timestamp);
EXPORT Pose3dWithTimestampMessage Pose3dToPose3dWithTimestampMessage(
    const Pose3d& pose, base::TimeDelta timestamp);

template <typename T, typename MessageType>
Pose3<T> Pose3WithTimestampMessageToPose3(const MessageType& message) {
  return {Point3MessageToPoint3<T>(message.point()),
          QuaternionMessageToQuaternion<T>(message.orientation())};
}

EXPORT Pose3f
Pose3fWithTimestampMessageToPose3f(const Pose3fWithTimestampMessage& message);
EXPORT Pose3d
Pose3dWithTimestampMessageToPose3d(const Pose3dWithTimestampMessage& message);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_POSE_H_