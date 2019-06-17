#include "felicia/drivers/camera/stereo_camera_interface.h"

namespace felicia {

StereoCameraInterface::StereoCameraInterface(
    const CameraDescriptor& camera_descriptor)
    : CameraInterfaceBase(camera_descriptor) {}

}  // namespace felicia