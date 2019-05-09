#include "felicia/core/lib/error/status.h"
#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/examples/learn/message_communication/common/cc/camera_flag.h"

namespace felicia {

Status GetCameraDescriptors(CameraDescriptors* camera_descriptors) {
  return Status(error::UNIMPLEMENTED, "Should compile with real vendor!");
}

bool PrintSupportCameraFormats(const CameraDescriptors& camera_descriptors,
                               const CameraFlag& delegate) {
  return false;
}

}  // namespace felicia