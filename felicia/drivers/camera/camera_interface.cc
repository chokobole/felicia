#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {

CameraInterface::CameraInterface(const CameraDescriptor& camera_descriptor)
    : camera_descriptor_(camera_descriptor) {}

CameraInterface::~CameraInterface() = default;

bool CameraInterface::IsInitialized() const {
  return camera_state_.IsInitialized();
}

bool CameraInterface::IsStarted() const { return camera_state_.IsStarted(); }

bool CameraInterface::IsStopped() const { return camera_state_.IsStopped(); }

}  // namespace felicia