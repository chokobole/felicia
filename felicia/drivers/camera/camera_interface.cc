#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {

CameraInterface::CameraInterface(const CameraDescriptor& camera_descriptor)
    : CameraInterfaceBase(camera_descriptor) {}

CameraFormat CameraInterface::camera_format() const { return camera_format_; }

}  // namespace felicia