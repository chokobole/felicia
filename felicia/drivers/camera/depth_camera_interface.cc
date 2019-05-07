#include "felicia/drivers/camera/depth_camera_interface.h"

namespace felicia {

DepthCameraInterface::DepthCameraInterface(
    const CameraDescriptor& camera_descriptor)
    : camera_descriptor_(camera_descriptor) {}

DepthCameraInterface::~DepthCameraInterface() = default;

}  // namespace felicia