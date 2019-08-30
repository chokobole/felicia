#ifndef FELICIA_SLAM_DATASET_SENSOR_DATA_H_
#define FELICIA_SLAM_DATASET_SENSOR_DATA_H_

#include "third_party/chromium/base/optional.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/geometry/pose.h"
#include "felicia/core/lib/unit/geometry/vector.h"
#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/drivers/camera/depth_camera_frame.h"
#include "felicia/drivers/imu/imu_frame.h"

namespace felicia {
namespace slam {

class EXPORT SensorData {
 public:
  enum DataType {
    DATA_TYPE_NONE = 0,
    DATA_TYPE_LEFT_CAMERA = 1 << 0,
    DATA_TYPE_RIGHT_CAMERA = 1 << 1,
    DATA_TYPE_LEFT_CAMERA_GRAY_SCALE = 1 << 2,
    DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE = 1 << 3,
    DATA_TYPE_DEPTH_CAMERA = 1 << 4,
    DATA_TYPE_IMU = 1 << 5,
    DATA_TYPE_LIDAR = 1 << 6,
    DATA_TYPE_ACCELERATION = 1 << 7,
    DATA_TYPE_GROUND_TRUTH_POSE = 1 << 8,
    DATA_TYPE_GROUND_TRUTH_POSITION = 1 << 9,
    DATA_TYPE_END = 1 << 10,
  };

  SensorData();
  SensorData(const SensorData& other);
  SensorData(SensorData&& other) noexcept;
  ~SensorData();
  SensorData& operator=(const SensorData& other);
  SensorData& operator=(SensorData&& other);

#define DECLARE_METHOD(Type, name)   \
  bool has_##name() const;           \
  void set_##name(const Type& name); \
  const Type& name() const&;         \
  Type&& name()&&

  DECLARE_METHOD(drivers::CameraFrame, left_camera_frame);
  DECLARE_METHOD(drivers::CameraFrame, right_camera_frame);
  DECLARE_METHOD(drivers::DepthCameraFrame, depth_camera_frame);
  DECLARE_METHOD(drivers::ImuFrame, imu_frame);
  DECLARE_METHOD(Vector3f, acceleration);
  DECLARE_METHOD(Vector3f, velocity);
  DECLARE_METHOD(Vector3f, angular_velocity);
  DECLARE_METHOD(Pose3f, pose);
  DECLARE_METHOD(Point3f, position);

#undef DECLARE_METHOD

  void set_timestamp(base::TimeDelta timestamp);
  base::TimeDelta timestamp() const;

 private:
  base::Optional<drivers::CameraFrame> left_camera_frame_;
  base::Optional<drivers::CameraFrame> right_camera_frame_;
  base::Optional<drivers::DepthCameraFrame> depth_camera_frame_;
  base::Optional<drivers::ImuFrame> imu_frame_;
  base::Optional<Vector3f> acceleration_;
  base::Optional<Vector3f> velocity_;
  base::Optional<Vector3f> angular_velocity_;
  base::Optional<Pose3f> pose_;
  base::Optional<Point3f> position_;
  base::TimeDelta timestamp_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_SENSOR_DATA_H_