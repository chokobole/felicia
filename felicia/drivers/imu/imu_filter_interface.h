#ifndef FELICIA_DRIVERS_IMU_IMU_FILTER_INTERFACE_H_
#define FELICIA_DRIVERS_IMU_IMU_FILTER_INTERFACE_H_

#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/quaternion.h"
#include "felicia/core/lib/unit/geometry/vector.h"

namespace felicia {

class EXPORT ImuFilterInterface {
 public:
  ImuFilterInterface();
  virtual ~ImuFilterInterface();

  virtual Quaternionf orientation() const = 0;
  virtual void UpdateAngularVelocity(const Vector3f& angular_velocity,
                                     ::base::TimeDelta timestamp) = 0;
  virtual void UpdateLinearAcceleration(
      const Vector3f& linear_acceleration) = 0;

  void UpdateAngularVelocity(float x, float y, float z,
                             ::base::TimeDelta timestamp);
  void UpdateLinearAcceleration(float x, float y, float z);

 protected:
  Quaternionf orientation_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_FILTER_INTERFACE_H_