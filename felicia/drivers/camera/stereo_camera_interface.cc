#include "felicia/drivers/camera/stereo_camera_interface.h"

namespace felicia {
namespace drivers {

StereoCameraInterface::StereoCameraInterface(
    const CameraDescriptor& camera_descriptor)
    : CameraInterfaceBase(camera_descriptor) {}

const CameraFormat& StereoCameraInterface::camera_format() const {
  return camera_format_;
}

}  // namespace drivers
}  // namespace felicia