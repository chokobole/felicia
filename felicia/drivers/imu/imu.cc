#include "felicia/drivers/imu/imu.h"

#include "felicia/core/lib/unit/geometry/geometry_util.h"

namespace felicia {

Imu::Imu()
    : orientation_(::Eigen::Quaternionf::Identity()),
      angular_velocity_(::Eigen::Vector3f::Zero()),
      linear_acceleration_(::Eigen::Vector3f::Zero()) {}

ImuMessage Imu::ToImuMessage() const {
  ImuMessage message;
  *message.mutable_orientation() =
      EigenQuarternionfToQuarternionMessage(orientation_);
  *message.mutable_angular_velocity() =
      EigenVec3fToVec3fMessage(angular_velocity_);
  *message.mutable_linear_acceleration() =
      EigenVec3fToVec3fMessage(linear_acceleration_);
  message.set_timestamp(timestamp_.InMicroseconds());
  return message;
}

void Imu::set_orientation(float w, float x, float y, float z) {
  orientation_.w() = w;
  orientation_.x() = x;
  orientation_.y() = y;
  orientation_.z() = z;
}

void Imu::set_angulary_veilocity(float x, float y, float z) {
  angular_velocity_(0) = x;
  angular_velocity_(1) = y;
  angular_velocity_(2) = z;
}

void Imu::set_linear_acceleration(float x, float y, float z) {
  linear_acceleration_(0) = x;
  linear_acceleration_(1) = y;
  linear_acceleration_(2) = z;
}

void Imu::set_orientation(const ::Eigen::Quaternionf& orientation) {
  orientation_ = orientation;
}

void Imu::set_angulary_veilocity(const ::Eigen::Vector3f& angular_velocity) {
  angular_velocity_ = angular_velocity;
}

void Imu::set_linear_acceleration(
    const ::Eigen::Vector3f& linear_acceleration) {
  linear_acceleration_ = linear_acceleration;
}

const ::Eigen::Quaternionf& Imu::orientation() const { return orientation_; }

const ::Eigen::Vector3f& Imu::angular_velocity() const {
  return angular_velocity_;
}

const ::Eigen::Vector3f& Imu::linear_acceleration() const {
  return linear_acceleration_;
}

void Imu::set_timestamp(::base::TimeDelta timestamp) { timestamp_ = timestamp; }

::base::TimeDelta Imu::timestamp() const { return timestamp_; }

}  // namespace felicia