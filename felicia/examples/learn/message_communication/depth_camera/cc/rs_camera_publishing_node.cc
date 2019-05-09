#include "felicia/drivers/camera/realsense/rs_camera_factory.h"

namespace felicia {

std::unique_ptr<DepthCameraInterface> MakeNewDepthCamera(
    const CameraDescriptor& camera_descriptor) {
  return RsCameraFactory::NewDepthCamera(camera_descriptor);
}

}  // namespace felicia