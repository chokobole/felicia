#include "felicia/drivers/camera/stereo_camera_interface.h"

namespace felicia {

StereoCameraInterface::StereoCameraInterface(
    const CameraDescriptor& camera_descriptor)
    : CameraInterfaceBase(camera_descriptor) {}

CameraFormat StereoCameraInterface::camera_format() const {
  return camera_format_;
}

}  // namespace felicia