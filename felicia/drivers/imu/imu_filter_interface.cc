#include "felicia/drivers/imu/imu_filter_interface.h"

namespace felicia {
namespace drivers {

ImuFilterInterface::ImuFilterInterface() = default;

ImuFilterInterface::~ImuFilterInterface() = default;

void ImuFilterInterface::UpdateAngularVelocity(float x, float y, float z,
                                               base::TimeDelta timestamp) {
  UpdateAngularVelocity(Vector3f{x, y, z}, timestamp);
}

void ImuFilterInterface::UpdateLinearAcceleration(float x, float y, float z) {
  UpdateLinearAcceleration(Vector3f{x, y, z});
}

}  // namespace drivers
}  // namespace felicia