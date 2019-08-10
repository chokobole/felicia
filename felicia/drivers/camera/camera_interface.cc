#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {
namespace drivers {

CameraInterface::CameraInterface(const CameraDescriptor& camera_descriptor)
    : CameraInterfaceBase(camera_descriptor) {}

const CameraFormat& CameraInterface::camera_format() const {
  return camera_format_;
}

}  // namespace drivers
}  // namespace felicia