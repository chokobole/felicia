#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {

CameraInterface::CameraInterface(const CameraDescriptor& camera_descriptor)
    : CameraInterfaceBase(camera_descriptor) {}

}  // namespace felicia