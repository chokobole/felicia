#include "felicia/drivers/imu/imu.h"

namespace felicia {

Imu::Imu() {
  orientation_.setZero();
  angular_velocity_.setZero();
  linear_acceleration_.setZero();
}

ImuMessage Imu::ToImuMessage() const {
  ImuMessage message;
  *message.mutable_orientation() = ToVec3fMessage(orientation_);
  *message.mutable_angular_velocity() = ToVec3fMessage(angular_velocity_);
  *message.mutable_linear_acceleration() = ToVec3fMessage(linear_acceleration_);
  return message;
}

void Imu::set_orientation(const float* data) {
  orientation_(0) = data[0];
  orientation_(1) = data[1];
  orientation_(2) = data[2];
}

void Imu::set_angulary_veilocity(const float* data) {
  angular_velocity_(0) = data[0];
  angular_velocity_(1) = data[1];
  angular_velocity_(2) = data[2];
}

void Imu::set_linear_accelration(const float* data) {
  linear_acceleration_(0) = data[0];
  linear_acceleration_(1) = data[1];
  linear_acceleration_(2) = data[2];
}

const ::Eigen::Vector3f& Imu::orientation() const {
  return orientation_;
}

const ::Eigen::Vector3f& Imu::angular_velocity() const {
  return angular_velocity_;
}

const ::Eigen::Vector3f& Imu::linear_acceleration() const {
  return linear_acceleration_;
}

// static
Vec3fMessage Imu::ToVec3fMessage(const ::Eigen::Vector3f& vec) {
  Vec3fMessage message;
  message.set_x(vec.x());
  message.set_y(vec.y());
  message.set_z(vec.z());
  return message;
}

}  // namespace felicia