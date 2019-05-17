#include "felicia/drivers/vendors/realsense/rs_camera_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/strings/strcat.h"

#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

// static
std::unique_ptr<RsCamera> RsCameraFactory::NewDepthCamera(
    const CameraDescriptor& descriptor) {
  return ::base::WrapUnique(new RsCamera(descriptor));
}

// static
Status RsCameraFactory::GetCameraDescriptors(
    CameraDescriptors* camera_descriptors) {
  DCHECK(camera_descriptors->empty());
  ::rs2::context context;
  ::rs2::device_list list = context.query_devices();
  for (auto&& dev : list) {
    const char* device_name = dev.get_info(RS2_CAMERA_INFO_NAME);
    const char* physical_port = dev.get_info(RS2_CAMERA_INFO_PHYSICAL_PORT);
    const char* product_id = dev.get_info(RS2_CAMERA_INFO_PRODUCT_ID);

    camera_descriptors->emplace_back(device_name, physical_port, product_id);
  }

  return Status::OK();
}

// static
Status RsCameraFactory::GetSupportedCameraFormats(
    const CameraDescriptor& camera_descriptor,
    RsCapabilityMap* rs_capability_map) {
  DCHECK(rs_capability_map->empty());
  ::rs2::device device;
  Status s = RsCamera::CreateDevice(camera_descriptor, &device);
  if (!s.ok()) return s;

  return RsCamera::CreateCapabilityMap(device, rs_capability_map);
}

}  // namespace felicia