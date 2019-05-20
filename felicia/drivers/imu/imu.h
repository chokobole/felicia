#ifndef FELICIA_DRIVERS_IMU_IMU_H_
#define FELICIA_DRIVERS_IMU_IMU_H_

#include "Eigen/Dense"
#include "third_party/chromium/base/callback.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/drivers/imu/imu_message.pb.h"

namespace felicia {

class EXPORT Imu {
 public:
  Imu();

  void set_orientation(float x, float y, float z);
  void set_angulary_veilocity(float x, float y, float z);
  void set_linear_accelration(float x, float y, float z);

  void set_orientation(const float* data);
  void set_angulary_veilocity(const float* data);
  void set_linear_accelration(const float* data);

  const ::Eigen::Vector3f& orientation() const;
  const ::Eigen::Vector3f& angular_velocity() const;
  const ::Eigen::Vector3f& linear_acceleration() const;

  void set_timestamp(::base::TimeDelta timestamp);
  ::base::TimeDelta timestamp() const;

  ImuMessage ToImuMessage() const;

 private:
  static Vec3fMessage ToVec3fMessage(const ::Eigen::Vector3f& vec);

  ::Eigen::Vector3f orientation_;
  ::Eigen::Vector3f angular_velocity_;
  ::Eigen::Vector3f linear_acceleration_;
  ::base::TimeDelta timestamp_;
};

typedef ::base::RepeatingCallback<void(const Imu&)> ImuCallback;

}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_H_