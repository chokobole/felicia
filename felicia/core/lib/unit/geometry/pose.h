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
  constexpr Pose(T x, T y, T theta) : position_(x, y), theta_(theta) {}
  constexpr Pose(const Point<T>& position, T theta)
      : position_(position), theta_(theta) {}
  constexpr Pose(const Pose& other) = default;
  Pose& operator=(const Pose& other) = default;

  constexpr Point<T>& position() { return position_; }
  constexpr const Point<T>& position() const { return position_; }

  void set_x(T x) { position_.set_x(x); }
  void set_y(T y) { position_.set_y(y); }
  void set_position(const Point<T>& position) { position_ = position; }
  void set_theta(T theta) { theta_ = theta; }

  constexpr T x() const { return position_.x(); }
  constexpr T y() const { return position_.y(); }
  constexpr T theta() const { return theta_; }

  constexpr bool IsValid() const {
    return position_.IsValid() && std::isfinite(theta_);
  }

  double Distance(const Pose& other) const {
    return position_.Distance(other.position_);
  }

  Pose Translate(const Vector<T>& vector) const {
    return {position_.Translate(vector), theta_};
  }
  Pose& TranslateInPlace(const Vector<T>& vector) {
    position_.TranslateInPlace(vector);
    return *this;
  }

  template <typename U>
  Pose Scale(U s) const {
    return Pose{position_.Scale(s), theta_};
  }
  template <typename U>
  Pose& ScaleInPlace(U s) {
    position_.ScaleInPlace(s);
    return *this;
  }

  Pose Rotate(T theta) { return Pose{position_, theta_ + theta}; }
  Pose& RotateInPlace(T theta) {
    theta_ += theta;
    return *this;
  }

  Transform<T> ToTransform() const {
    Transform<T> transform;
    transform.AddRotation(theta_).AddTranslate(position_.x(), position_.y());
    return transform;
  }

 private:
  Point<T> position_;
  T theta_;
};

template <typename T>
inline bool operator==(const Pose<T>& lhs, const Pose<T>& rhs) {
  return lhs.position() == rhs.position() && lhs.theta() == rhs.theta();
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
  *message.mutable_position() =
      PointToPointMessage<PointMessageType>(pose.position());
  message.set_theta(pose.theta());
  return message;
}

FEL_EXPORT PosefMessage PosefToPosefMessage(const Posef& pose);
FEL_EXPORT PosedMessage PosedToPosedMessage(const Posed& pose);

template <typename T, typename MessageType>
Pose<T> PoseMessageToPose(const MessageType& message) {
  return {PointMessageToPoint<T>(message.position()), message.theta()};
}

FEL_EXPORT Posef PosefMessageToPosef(const PosefMessage& message);
FEL_EXPORT Posed PosedMessageToPosed(const PosedMessage& message);

template <typename PoseWithTimestampMessageType, typename PointMessageType,
          typename T>
PoseWithTimestampMessageType PoseToPoseWithTimestampMessage(
    const Pose<T>& pose, base::TimeDelta timestamp) {
  PoseWithTimestampMessageType message;
  *message.mutable_position() =
      PointToPointMessage<PointMessageType>(pose.position());
  message.set_theta(pose.theta());
  message.set_timestamp(timestamp.InMicroseconds());
  return message;
}

FEL_EXPORT PosefWithTimestampMessage
PosefToPosefWithTimestampMessage(const Posef& pose, base::TimeDelta timestamp);
FEL_EXPORT PosedWithTimestampMessage
PosedToPosedWithTimestampMessage(const Posed& pose, base::TimeDelta timestamp);

template <typename T, typename MessageType>
Pose<T> PoseWithTimestampMessageToPose(const MessageType& message) {
  return {PointMessageToPoint<T>(message.position()), message.theta()};
}

FEL_EXPORT Posef
PosefWithTimestampMessageToPosef(const PosefWithTimestampMessage& message);
FEL_EXPORT Posed
PosedWithTimestampMessageToPosed(const PosedWithTimestampMessage& message);

template <typename T>
class Pose3 {
 public:
  constexpr Pose3() = default;
  constexpr Pose3(T x, T y, T z, const Quaternion<T>& orientation)
      : position_(x, y, z), orientation_(orientation) {}
  constexpr Pose3(const Point3<T>& position, const Quaternion<T>& orientation)
      : position_(position), orientation_(orientation) {}
  constexpr Pose3(const Pose3& other) = default;
  Pose3& operator=(const Pose3& other) = default;

  constexpr Point3<T>& position() { return position_; }
  constexpr const Point3<T>& position() const { return position_; }
  constexpr Quaternion<T>& orientation() { return orientation_; }
  constexpr const Quaternion<T>& orientation() const { return orientation_; }

  void set_x(T x) { position_.set_x(x); }
  void set_y(T y) { position_.set_y(y); }
  void set_z(T z) { position_.set_z(z); }
  void set_position(const Point3<T>& position) { position_ = position; }
  void set_orientation(const Quaternion<T>& orientation) {
    orientation_ = orientation;
  }

  constexpr T x() const { return position_.x(); }
  constexpr T y() const { return position_.y(); }
  constexpr T z() const { return position_.z(); }

  constexpr bool IsValid() const {
    return position_.IsValid() && orientation_.IsValid();
  }

  double Distance(const Pose3& other) const {
    return position_.Distance(other.position_);
  }

  Pose3 Translate(const Vector3<T>& vector) const {
    return {position_.Translate(vector), orientation_};
  }
  Pose3& TranslateInPlace(const Vector3<T>& vector) {
    position_.TranslateInPlace(vector);
    return *this;
  }

  template <typename U>
  Pose3 Scale(U s) const {
    return Pose3{position_.Scale(s), orientation_};
  }
  template <typename U>
  Pose3& ScaleInPlace(U s) {
    position_.ScaleInPlace(s);
    return *this;
  }

  Transform3<T> ToTransform() const {
    Transform3<T> transform;
    transform.AddRotation(orientation_)
        .AddTranslate(position_.x(), position_.y(), position_.z());
    return transform;
  }

 private:
  Point3<T> position_;
  Quaternion<T> orientation_;
};

template <typename T>
inline bool operator==(const Pose3<T>& lhs, const Pose3<T>& rhs) {
  return lhs.position() == rhs.position() &&
         lhs.orientation() == rhs.orientation();
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
  *message.mutable_position() =
      Point3ToPoint3Message<Point3MessageType>(pose.position());
  *message.mutable_orientation() =
      QuaternionToQuaternionMessage<QuaternionMessageType>(pose.orientation());
  return message;
}

FEL_EXPORT Pose3fMessage Pose3fToPose3fMessage(const Pose3f& pose);
FEL_EXPORT Pose3dMessage Pose3dToPose3dMessage(const Pose3d& pose);

template <typename T, typename MessageType>
Pose3<T> Pose3MessageToPose3(const MessageType& message) {
  return {Point3MessageToPoint3<T>(message.position()),
          QuaternionMessageToQuaternion<T>(message.orientation())};
}

FEL_EXPORT Pose3f Pose3fMessageToPose3f(const Pose3fMessage& message);
FEL_EXPORT Pose3d Pose3dMessageToPose3d(const Pose3dMessage& message);

template <typename Pose3WithTimestampMessageType, typename Point3MessageType,
          typename QuaternionMessageType, typename T>
Pose3WithTimestampMessageType Pose3ToPose3WithTimestampMessage(
    const Pose3<T>& pose, base::TimeDelta timestamp) {
  Pose3WithTimestampMessageType message;
  *message.mutable_position() =
      Point3ToPoint3Message<Point3MessageType>(pose.position());
  *message.mutable_orientation() =
      QuaternionToQuaternionMessage<QuaternionMessageType>(pose.orientation());
  message.set_timestamp(timestamp.InMicroseconds());
  return message;
}

FEL_EXPORT Pose3fWithTimestampMessage Pose3fToPose3fWithTimestampMessage(
    const Pose3f& pose, base::TimeDelta timestamp);
FEL_EXPORT Pose3dWithTimestampMessage Pose3dToPose3dWithTimestampMessage(
    const Pose3d& pose, base::TimeDelta timestamp);

template <typename T, typename MessageType>
Pose3<T> Pose3WithTimestampMessageToPose3(const MessageType& message) {
  return {Point3MessageToPoint3<T>(message.position()),
          QuaternionMessageToQuaternion<T>(message.orientation())};
}

FEL_EXPORT Pose3f
Pose3fWithTimestampMessageToPose3f(const Pose3fWithTimestampMessage& message);
FEL_EXPORT Pose3d
Pose3dWithTimestampMessageToPose3d(const Pose3dWithTimestampMessage& message);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_POSE_H_