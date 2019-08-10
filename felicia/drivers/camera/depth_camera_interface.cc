#include "felicia/drivers/camera/depth_camera_interface.h"

namespace felicia {
namespace drivers {

DepthCameraInterface::DepthCameraInterface(
    const CameraDescriptor& camera_descriptor)
    : CameraInterfaceBase(camera_descriptor) {}

const CameraFormat& DepthCameraInterface::color_format() const {
  return color_format_;
}

const CameraFormat& DepthCameraInterface::depth_format() const {
  return depth_format_;
}

}  // namespace drivers
}  // namespace felicia