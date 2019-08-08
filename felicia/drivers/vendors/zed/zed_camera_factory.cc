#include "felicia/drivers/vendors/zed/zed_camera_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/util/command_line_interface/text_constants.h"
#include "felicia/drivers/camera/camera_factory.h"

namespace felicia {

// static
std::unique_ptr<ZedCamera> ZedCameraFactory::NewStereoCamera(
    const ZedCameraDescriptor& camera_descriptor) {
  return base::WrapUnique(new ZedCamera(camera_descriptor));
}

// static
Status ZedCameraFactory::GetCameraDescriptors(
    ZedCameraDescriptors* camera_descriptors) {
  CameraDescriptors nt_camera_descriptors;
  CameraFactory::GetCameraDescriptors(&nt_camera_descriptors);

#if defined(OS_WIN) && !defined(NDEBUG)
  // If you look at declaration function
  // "static std::vector<sl::DeviceProperties> getDeviceList();",
  // it says like below.
  //   \warning As this function returns an std::vector, it is only safe to use
  //   in Release mode (not Debug). \n This is due to a known compatibility
  //   issue between release (the SDK) and debug (your app) implementations of
  //   std::vector.
  // So if it is built with debug mode, we tried guess with camera name
  std::cerr
      << kYellowWarning
      << "This might be wrong due to API is not supported from stereo labs. To "
         "make it correct, you have to build with release mode."
      << std::endl;
  int id = 0;
  for (auto& nt_camera_descriptor : nt_camera_descriptors) {
    if (strings::StartsWith(nt_camera_descriptor.display_name(), "ZED")) {
      camera_descriptors->emplace_back(nt_camera_descriptor, id++);
    }
  }
#else
  std::vector<sl::DeviceProperties> deviceList = sl::Camera::getDeviceList();
  for (auto& device : deviceList) {
    if (device.camera_state == sl::CAMERA_STATE_AVAILABLE) {
      for (auto& nt_camera_descriptor : nt_camera_descriptors) {
        if (ZedCamera::IsSameId(nt_camera_descriptor.device_id(),
                                device.path.c_str())) {
          camera_descriptors->emplace_back(
              nt_camera_descriptor.display_name(), device.path.c_str(),
              nt_camera_descriptor.model_id(), device.id);
        }
      }
    }
  }
#endif
  return Status::OK();
}

// static
Status ZedCameraFactory::GetSupportedCameraFormats(
    const ZedCameraDescriptor& camera_descriptor,
    CameraFormats* camera_formats) {
  DCHECK(camera_formats->empty());
  for (auto& capability : kZedCapabilities) {
    camera_formats->push_back(ConvertToCameraFormat(capability));
  }
  return Status::OK();
}

}  // namespace felicia