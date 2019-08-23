#ifndef FELICIA_SLAM_DATASET_SENSOR_META_DATA_H_
#define FELICIA_SLAM_DATASET_SENSOR_META_DATA_H_

#include "third_party/chromium/base/optional.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/drivers/camera/camera_format.h"
#include "felicia/slam/types.h"

namespace felicia {
namespace slam {

class EXPORT SensorMetaData {
 public:
#define DECLARE_METHOD(Type, name)   \
  bool has_##name() const;           \
  void set_##name(const Type& name); \
  const Type& name() const

  DECLARE_METHOD(drivers::CameraFormat, color_camera_format);
  DECLARE_METHOD(drivers::CameraFormat, depth_camera_format);
  DECLARE_METHOD(EigenCameraMatrixd, left_K);
  DECLARE_METHOD(EigenCameraMatrixd, right_K);
  DECLARE_METHOD(EigenDistortionMatrixd, left_D);
  DECLARE_METHOD(EigenDistortionMatrixd, right_D);
  DECLARE_METHOD(EigenProjectionMatrixd, left_P);
  DECLARE_METHOD(EigenProjectionMatrixd, right_P);

#undef DECLARE_METHOD

 private:
  base::Optional<drivers::CameraFormat> color_camera_format_;
  base::Optional<drivers::CameraFormat> depth_camera_format_;
  base::Optional<EigenCameraMatrixd> left_K_;
  base::Optional<EigenCameraMatrixd> right_K_;
  base::Optional<EigenDistortionMatrixd> left_D_;
  base::Optional<EigenDistortionMatrixd> right_D_;
  base::Optional<EigenProjectionMatrixd> left_P_;
  base::Optional<EigenProjectionMatrixd> right_P_;
};

}  // namespace slam
}  // namespace felicia

#endif  // FELICIA_SLAM_DATASET_SENSOR_META_DATA_H_