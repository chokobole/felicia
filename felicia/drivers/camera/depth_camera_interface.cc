#include "felicia/drivers/camera/depth_camera_interface.h"

namespace felicia {

DepthCameraInterface::DepthCameraInterface(
    const CameraDescriptor& camera_descriptor)
    : CameraInterfaceBase(camera_descriptor) {}

CameraFormat DepthCameraInterface::color_format() const {
  return color_format_;
}

CameraFormat DepthCameraInterface::depth_format() const {
  return depth_format_;
}

}  // namespace felicia