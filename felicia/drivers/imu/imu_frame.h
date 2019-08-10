#ifndef FELICIA_DRIVERS_IMU_IMU_FRAME_H_
#define FELICIA_DRIVERS_IMU_IMU_FRAME_H_

#include "third_party/chromium/base/callback.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/quaternion.h"
#include "felicia/core/lib/unit/geometry/vector.h"
#include "felicia/drivers/imu/imu_frame_message.pb.h"

namespace felicia {
namespace drivers {

class EXPORT ImuFrame {
 public:
  ImuFrame();
  ImuFrame(const Quaternionf& orientation, const Vector3f& angular_velocity,
           const Vector3f& linear_acceleration, base::TimeDelta timestamp);

  void set_orientation(float w, float x, float y, float z);
  void set_angulary_veilocity(float x, float y, float z);
  void set_linear_acceleration(float x, float y, float z);

  void set_orientation(const Quaternionf& orientation);
  void set_angulary_veilocity(const Vector3f& angular_velocity);
  void set_linear_acceleration(const Vector3f& linear_acceleration);

  const Quaternionf& orientation() const;
  const Vector3f& angular_velocity() const;
  const Vector3f& linear_acceleration() const;

  void set_timestamp(base::TimeDelta timestamp);
  base::TimeDelta timestamp() const;

  ImuFrameMessage ToImuFrameMessage() const;
  static ImuFrame FromImuFrameMessage(const ImuFrameMessage& message);

 private:
  Quaternionf orientation_;
  Vector3f angular_velocity_;
  Vector3f linear_acceleration_;
  base::TimeDelta timestamp_;
};

typedef base::RepeatingCallback<void(const ImuFrame&)> ImuFrameCallback;

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_FRAME_H_