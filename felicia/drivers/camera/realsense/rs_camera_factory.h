#ifndef FELICIA_DRIVERS_CAMERA_REALSENSE_RS_CAMERA_FACTORY_H_
#define FELICIA_DRIVERS_CAMERA_REALSENSE_RS_CAMERA_FACTORY_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/drivers/camera/realsense/rs_camera.h"
#include "felicia/drivers/camera/realsense/rs_stream_info.h"

namespace felicia {

class RsCameraFactory {
 public:
  static std::unique_ptr<DepthCameraInterface> NewDepthCamera(
      const CameraDescriptor& descriptor);

  static Status GetCameraDescriptors(CameraDescriptors* camera_descriptors);

  static Status GetSupportedCameraFormats(
      const CameraDescriptor& camera_descriptor,
      RsCapabilityMap* rs_capability_map);

  DISALLOW_COPY_AND_ASSIGN(RsCameraFactory);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_REALSENSE_RS_CAMERA_FACTORY_H_