#ifndef FELICIA_DRIVERS_IMU_IMU_FILTER_INTERFACE_H_
#define FELICIA_DRIVERS_IMU_IMU_FILTER_INTERFACE_H_

#include "Eigen/Core"
#include "Eigen/Geometry"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

class EXPORT ImuFilterInterface {
 public:
  ImuFilterInterface();
  virtual ~ImuFilterInterface();

  virtual ::Eigen::Quaternionf orientation() const = 0;
  virtual void UpdateAngularVelocity(float x, float y, float z,
                                     ::base::TimeDelta timestamp) = 0;
  virtual void UpdateLinearAcceleration(float x, float y, float z) = 0;

  void UpdateAngularVelocity(const ::Eigen::Vector3f& angular_velocity,
                             ::base::TimeDelta timestamp);
  void UpdateLinearAcceleration(const ::Eigen::Vector3f& linear_acceleration);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 protected:
  Eigen::Quaternionf orientation_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_FILTER_INTERFACE_H_