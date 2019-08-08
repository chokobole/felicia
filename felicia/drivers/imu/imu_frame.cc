#include "felicia/drivers/imu/imu_frame.h"

namespace felicia {

ImuFrame::ImuFrame() = default;

ImuFrame::ImuFrame(const Quaternionf& orientation,
                   const Vector3f& angular_velocity,
                   const Vector3f& linear_acceleration,
                   base::TimeDelta timestamp)
    : orientation_(orientation),
      angular_velocity_(angular_velocity),
      linear_acceleration_(linear_acceleration),
      timestamp_(timestamp) {}

void ImuFrame::set_orientation(float w, float x, float y, float z) {
  orientation_.set_xyzw(x, y, z, w);
}

void ImuFrame::set_angulary_veilocity(float x, float y, float z) {
  angular_velocity_.set_xyz(x, y, z);
}

void ImuFrame::set_linear_acceleration(float x, float y, float z) {
  linear_acceleration_.set_xyz(x, y, z);
}

void ImuFrame::set_orientation(const Quaternionf& orientation) {
  orientation_ = orientation;
}

void ImuFrame::set_angulary_veilocity(const Vector3f& angular_velocity) {
  angular_velocity_ = angular_velocity;
}

void ImuFrame::set_linear_acceleration(const Vector3f& linear_acceleration) {
  linear_acceleration_ = linear_acceleration;
}

const Quaternionf& ImuFrame::orientation() const { return orientation_; }

const Vector3f& ImuFrame::angular_velocity() const { return angular_velocity_; }

const Vector3f& ImuFrame::linear_acceleration() const {
  return linear_acceleration_;
}

void ImuFrame::set_timestamp(base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

base::TimeDelta ImuFrame::timestamp() const { return timestamp_; }

ImuFrameMessage ImuFrame::ToImuFrameMessage() const {
  ImuFrameMessage message;
  *message.mutable_orientation() =
      QuaternionfToQuaternionfMessage(orientation_);
  *message.mutable_angular_velocity() =
      Vector3fToVector3fMessage(angular_velocity_);
  *message.mutable_linear_acceleration() =
      Vector3fToVector3fMessage(linear_acceleration_);
  message.set_timestamp(timestamp_.InMicroseconds());
  return message;
}

// static
ImuFrame ImuFrame::FromImuFrameMessage(const ImuFrameMessage& message) {
  return {QuaternionfMessageToQuaternionf(message.orientation()),
          Vector3fMessageToVector3f(message.angular_velocity()),
          Vector3fMessageToVector3f(message.angular_velocity()),
          base::TimeDelta::FromMicroseconds(message.timestamp())};
}

}  // namespace felicia