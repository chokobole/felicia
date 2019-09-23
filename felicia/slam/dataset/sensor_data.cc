#include "felicia/slam/dataset/sensor_data.h"

namespace felicia {
namespace slam {

SensorData::SensorData() = default;

SensorData::SensorData(const SensorData& other) = default;

SensorData::SensorData(SensorData&& other) noexcept
    : left_camera_frame_(std::move(other.left_camera_frame_)),
      right_camera_frame_(std::move(other.right_camera_frame_)),
      depth_camera_frame_(std::move(other.depth_camera_frame_)),
      imu_frame_(std::move(other.imu_frame_)),
      lidar_frame_(std::move(other.lidar_frame_)),
      pointcloud_(std::move(other.pointcloud_)),
      acceleration_(std::move(other.acceleration_)),
      velocity_(std::move(other.velocity_)),
      angular_velocity_(std::move(other.angular_velocity_)),
      pose_(std::move(other.pose_)),
      position_(std::move(other.position_)),
      timestamp_(other.timestamp_) {}

SensorData::~SensorData() = default;

SensorData& SensorData::operator=(const SensorData& other) = default;
SensorData& SensorData::operator=(SensorData&& other) = default;

#define DEFINE_METHOD(Type, name)                                         \
  bool SensorData::has_##name() const { return name##_.has_value(); }     \
  void SensorData::set_##name(const Type& name) { name##_ = name; }       \
  void SensorData::set_##name(Type&& name) { name##_ = std::move(name); } \
  const Type& SensorData::name() const& { return name##_.value(); }       \
  Type&& SensorData::name()&& { return std::move(name##_).value(); }

DEFINE_METHOD(drivers::CameraFrame, left_camera_frame)
DEFINE_METHOD(drivers::CameraFrame, right_camera_frame)
DEFINE_METHOD(drivers::DepthCameraFrame, depth_camera_frame)
DEFINE_METHOD(drivers::ImuFrame, imu_frame)
DEFINE_METHOD(drivers::LidarFrame, lidar_frame)
DEFINE_METHOD(map::Pointcloud, pointcloud)
DEFINE_METHOD(Vector3f, acceleration)
DEFINE_METHOD(Vector3f, velocity)
DEFINE_METHOD(Vector3f, angular_velocity)
DEFINE_METHOD(Pose3f, pose)
DEFINE_METHOD(Point3f, position)

#undef DEFINE_METHOD

void SensorData::set_timestamp(base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}
base::TimeDelta SensorData::timestamp() const { return timestamp_; }

// static
std::string SensorData::ToString(DataType data_type) {
  switch (data_type) {
    case DATA_TYPE_NONE:
      return "DATA_TYPE_NONE";
    case DATA_TYPE_LEFT_CAMERA:
      return "DATA_TYPE_LEFT_CAMERA";
    case DATA_TYPE_RIGHT_CAMERA:
      return "DATA_TYPE_RIGHT_CAMERA";
    case DATA_TYPE_LEFT_CAMERA_GRAY_SCALE:
      return "DATA_TYPE_LEFT_CAMERA_GRAY_SCALE";
    case DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE:
      return "DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE";
    case DATA_TYPE_DEPTH_CAMERA:
      return "DATA_TYPE_DEPTH_CAMERA";
    case DATA_TYPE_IMU:
      return "DATA_TYPE_IMU";
    case DATA_TYPE_POINTCLOUD:
      return "DATA_TYPE_POINTCLOUD";
    case DATA_TYPE_LIDAR:
      return "DATA_TYPE_LIDAR";
    case DATA_TYPE_ACCELERATION:
      return "DATA_TYPE_ACCELERATION";
    case DATA_TYPE_GROUND_TRUTH_POSE:
      return "DATA_TYPE_GROUND_TRUTH_POSE";
    case DATA_TYPE_GROUND_TRUTH_POSITION:
      return "DATA_TYPE_GROUND_TRUTH_POSITION";
    case DATA_TYPE_END:
      return "DATA_TYPE_END";
  }
}

}  // namespace slam
}  // namespace felicia