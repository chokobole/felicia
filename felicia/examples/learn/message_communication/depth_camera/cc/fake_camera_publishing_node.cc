#include "felicia/drivers/camera/depth_camera_interface.h"

namespace felicia {

std::unique_ptr<DepthCameraInterface> MakeNewDepthCamera(
    const CameraDescriptor& camera_descriptor) {
  return nullptr;
}

}  // namespace felicia