#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {

CameraInterface::CameraInterface(const CameraDescriptor& camera_descriptor)
    : camera_descriptor_(camera_descriptor) {}

CameraInterface::~CameraInterface() = default;

}  // namespace felicia