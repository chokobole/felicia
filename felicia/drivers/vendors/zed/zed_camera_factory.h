#ifndef FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FACTORY_H_
#define FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FACTORY_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/drivers/vendors/zed/zed_camera.h"

namespace felicia {

class ZedCameraFactory {
 public:
  static std::unique_ptr<ZedCamera> NewStereoCamera(
      const CameraDescriptor& camera_descriptor);

  static Status GetCameraDescriptors(CameraDescriptors* camera_descriptors);

  static Status GetSupportedCameraFormats(
      const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats);

  DISALLOW_COPY_AND_ASSIGN(ZedCameraFactory);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FACTORY_H_