#include "felicia/drivers/camera/depth_camera_interface.h"

namespace felicia {

DepthCameraInterface::DepthCameraInterface(
    const CameraDescriptor& camera_descriptor)
    : camera_descriptor_(camera_descriptor) {}

DepthCameraInterface::~DepthCameraInterface() = default;

Status DepthCameraInterface::SetCameraSettings(
    const CameraSettings& camera_settings) {
  return errors::Unimplemented("Not implemented yet.");
}

Status DepthCameraInterface::GetCameraSettingsInfo(
    CameraSettingsInfoMessage* camera_settings) {
  return errors::Unimplemented("Not implemented yet.");
}

bool DepthCameraInterface::IsInitialized() const {
  return camera_state_.IsInitialized();
}

bool DepthCameraInterface::IsStarted() const {
  return camera_state_.IsStarted();
}

bool DepthCameraInterface::IsStopped() const {
  return camera_state_.IsStopped();
}

}  // namespace felicia