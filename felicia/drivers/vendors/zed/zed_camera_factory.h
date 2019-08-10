#ifndef FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FACTORY_H_
#define FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FACTORY_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/drivers/vendors/zed/zed_camera.h"
#include "felicia/drivers/vendors/zed/zed_camera_descriptor.h"

namespace felicia {
namespace drivers {

class ZedCameraFactory {
 public:
  static std::unique_ptr<ZedCamera> NewStereoCamera(
      const ZedCameraDescriptor& camera_descriptor);

  static Status GetCameraDescriptors(ZedCameraDescriptors* camera_descriptors);

  static Status GetSupportedCameraFormats(
      const ZedCameraDescriptor& camera_descriptor,
      CameraFormats* camera_formats);

  DISALLOW_COPY_AND_ASSIGN(ZedCameraFactory);
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FACTORY_H_