#ifndef FELICIA_SLAM_DATASET_SENSOR_DATA_H_
#define FELICIA_SLAM_DATASET_SENSOR_DATA_H_

#include "third_party/chromium/base/optional.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/pose.h"
#include "felicia/core/lib/unit/geometry/vector.h"
#include "felicia/drivers/imu/imu_frame.h"

namespace felicia {
namespace slam {

class EXPORT SensorData {
 public:
#define DECLARE_METHOD(Type, name)   \
  bool has_##name() const;           \
  void set_##name(const Type& name); \
  const Type& name() const

  DECLARE_METHOD(std::string, left_image_filename);
  DECLARE_METHOD(std::string, right_image_filename);
  DECLARE_METHOD(std::string, depth_image_filename);
  DECLARE_METHOD(drivers::ImuFrame, imu_frame);
  DECLARE_METHOD(Vector3f, acceleration);
  DECLARE_METHOD(Pose3f, pose);

#undef DECLARE_METHOD

  void set_timestamp(double timestamp);
  double timestamp() const;

 private:
  // TODO: We should read the image and give users a CameraFrame
  base::Optional<std::string> left_image_filename_;
  base::Optional<std::string> right_image_filename_;
  base::Optional<std::string> depth_image_filename_;
  base::Optional<drivers::ImuFrame> imu_frame_;
  base::Optional<Vector3f> acceleration_;
  base::Optional<Pose3f> pose_;
  double timestamp_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_SENSOR_DATA_H_