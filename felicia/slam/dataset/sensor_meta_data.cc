#include "felicia/slam/dataset/sensor_meta_data.h"

namespace felicia {
namespace slam {

#define DEFINE_METHOD(Type, name)                                         \
  bool SensorMetaData::has_##name() const { return name##_.has_value(); } \
  void SensorMetaData::set_##name(const Type& name) { name##_ = name; }   \
  const Type& SensorMetaData::name() const { return name##_.value(); }

DEFINE_METHOD(drivers::CameraFormat, color_camera_format)
DEFINE_METHOD(drivers::CameraFormat, depth_camera_format)
DEFINE_METHOD(EigenCameraMatrixd, left_K)
DEFINE_METHOD(EigenCameraMatrixd, right_K)
DEFINE_METHOD(EigenDistortionMatrixd, left_D)
DEFINE_METHOD(EigenDistortionMatrixd, right_D)
DEFINE_METHOD(EigenProjectionMatrixd, left_P)
DEFINE_METHOD(EigenProjectionMatrixd, right_P)

#undef DEFINE_METHOD

}  // namespace slam
}  // namespace felicia