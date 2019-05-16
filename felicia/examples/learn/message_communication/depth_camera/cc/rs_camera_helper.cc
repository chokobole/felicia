#include "felicia/drivers/vendors/realsense/rs_camera_factory.h"
#include "felicia/examples/learn/message_communication/common/cc/camera_flag.h"

namespace felicia {

namespace {

void Print(const RsCapabilityList& rs_capabilities) {
  size_t i = 0;
  for (auto& rs_capability : rs_capabilities) {
    std::cout << "[" << i << "] " << rs_capability.supported_format
              << std::endl;
    i++;
  }
}

void Print(const RsCapabilityMap& rs_capability_map) {
  for (auto& v : rs_capability_map) {
    std::cout << v.first.stream_type << "(" << v.first.stream_index << ")"
              << std::endl;
    Print(v.second);
  }
}

}  // namespace

Status GetCameraDescriptors(CameraDescriptors* camera_descriptors) {
  return RsCameraFactory::GetCameraDescriptors(camera_descriptors);
}

bool PrintSupportCameraFormats(const CameraDescriptors& camera_descriptors,
                               const CameraFlag& delegate) {
  RsCapabilityMap rs_capability_map;
  Status s = RsCameraFactory::GetSupportedCameraFormats(
      camera_descriptors[delegate.device_index_flag()->value()],
      &rs_capability_map);
  if (!s.ok()) {
    std::cerr << kRedError << s << std::endl;
    return false;
  }
  Print(rs_capability_map);
  return true;
}

}  // namespace felicia