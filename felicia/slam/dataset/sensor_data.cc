#include "felicia/slam/dataset/sensor_data.h"

namespace felicia {
namespace slam {

#define DEFINE_METHOD(Type, name)                                     \
  bool SensorData::has_##name() const { return name##_.has_value(); } \
  void SensorData::set_##name(const Type& name) { name##_ = name; }   \
  const Type& SensorData::name() const { return name##_.value(); }

DEFINE_METHOD(std::string, left_image_filename)
DEFINE_METHOD(std::string, right_image_filename)
DEFINE_METHOD(std::string, depth_image_filename)
DEFINE_METHOD(drivers::ImuFrame, imu_frame)
DEFINE_METHOD(Vector3f, acceleration)
DEFINE_METHOD(Vector3f, velocity)
DEFINE_METHOD(Vector3f, angular_velocity)
DEFINE_METHOD(Point3f, point)
DEFINE_METHOD(Pose3f, pose)

#undef DEFINE_METHOD

void SensorData::set_timestamp(double timestamp) { timestamp_ = timestamp; }
double SensorData::timestamp() const { return timestamp_; }

}  // namespace slam
}  // namespace felicia