#ifndef FELICIA_DRIVERS_IMU_IMU_H_
#define FELICIA_DRIVERS_IMU_IMU_FRAME_H_

#include "Eigen/Core"
#include "Eigen/Geometry"
#include "third_party/chromium/base/callback.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/drivers/imu/imu_frame_message.pb.h"

namespace felicia {

class EXPORT ImuFrame {
 public:
  ImuFrame();

  void set_orientation(float w, float x, float y, float z);
  void set_angulary_veilocity(float x, float y, float z);
  void set_linear_acceleration(float x, float y, float z);

  void set_orientation(const ::Eigen::Quaternionf& orientation);
  void set_angulary_veilocity(const ::Eigen::Vector3f& angular_velocity);
  void set_linear_acceleration(const ::Eigen::Vector3f& linear_acceleration);

  const ::Eigen::Quaternionf& orientation() const;
  const ::Eigen::Vector3f& angular_velocity() const;
  const ::Eigen::Vector3f& linear_acceleration() const;

  void set_timestamp(::base::TimeDelta timestamp);
  ::base::TimeDelta timestamp() const;

  ImuFrameMessage ToImuFrameMessage() const;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 private:
  ::Eigen::Quaternionf orientation_;
  ::Eigen::Vector3f angular_velocity_;
  ::Eigen::Vector3f linear_acceleration_;
  ::base::TimeDelta timestamp_;
};

typedef ::base::RepeatingCallback<void(const ImuFrame&)> ImuFrameCallback;

}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_H_