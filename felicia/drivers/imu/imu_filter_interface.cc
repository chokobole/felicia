#include "felicia/drivers/imu/imu_filter_interface.h"

namespace felicia {

ImuFilterInterface::ImuFilterInterface()
    : orientation_(::Eigen::Quaternionf::Identity()) {}

ImuFilterInterface::~ImuFilterInterface() = default;

void ImuFilterInterface::UpdateAngularVelocity(
    const ::Eigen::Vector3f& angular_velocity, ::base::TimeDelta timestamp) {
  UpdateAngularVelocity(angular_velocity.x(), angular_velocity.y(),
                        angular_velocity.z(), timestamp);
}

void ImuFilterInterface::UpdateLinearAcceleration(
    const ::Eigen::Vector3f& linear_acceleration) {
  UpdateLinearAcceleration(linear_acceleration.x(), linear_acceleration.y(),
                           linear_acceleration.z());
}

}  // namespace felicia