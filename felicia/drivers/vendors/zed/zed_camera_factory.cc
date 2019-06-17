#include "felicia/drivers/vendors/zed/zed_camera_factory.h"

#include "felicia/core/lib/strings/str_util.h"
#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/build/build_config.h"

#if defined(OS_LINUX)
#include "felicia/drivers/camera/linux/v4l2_camera.h"
#endif

namespace felicia {

// static
std::unique_ptr<ZedCamera> ZedCameraFactory::NewStereoCamera(
    const CameraDescriptor& camera_descriptor) {
  return ::base::WrapUnique(new ZedCamera(camera_descriptor));
}

// static
Status ZedCameraFactory::GetCameraDescriptors(
    CameraDescriptors* camera_descriptors) {
  CameraDescriptors nt_camera_descriptors;
#if defined(OS_LINUX)
  V4l2Camera::GetCameraDescriptors(&nt_camera_descriptors);
#endif

  std::vector<::sl::DeviceProperties> deviceList =
      ::sl::Camera::getDeviceList();
  for (auto& device : deviceList) {
    if (device.camera_state == ::sl::CAMERA_STATE_AVAILABLE) {
      for (auto& nt_camera_descriptor : nt_camera_descriptors) {
        if (strings::Equals(nt_camera_descriptor.device_id(),
                            device.path.c_str())) {
          camera_descriptors->push_back(nt_camera_descriptor);
        }
      }
    }
  }
  return Status::OK();
}

// static
Status ZedCameraFactory::GetSupportedCameraFormats(
    const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats) {
  DCHECK(camera_formats->empty());
  for (auto& capability : kZedCapabilities) {
    camera_formats->push_back(ConvertToCameraFormat(capability));
  }
  return Status::OK();
}

}  // namespace felicia