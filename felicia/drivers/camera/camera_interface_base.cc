#include "felicia/drivers/camera/camera_interface_base.h"

namespace felicia {

CameraInterfaceBase::CameraInterfaceBase(
    const CameraDescriptor& camera_descriptor)
    : camera_descriptor_(camera_descriptor) {}

CameraInterfaceBase::~CameraInterfaceBase() = default;

Status CameraInterfaceBase::SetCameraSettings(
    const CameraSettings& camera_settings) {
  return errors::Unimplemented("Not implemented yet.");
}

Status CameraInterfaceBase::GetCameraSettingsInfo(
    CameraSettingsInfoMessage* camera_settings) {
  return errors::Unimplemented("Not implemented yet.");
}

bool CameraInterfaceBase::IsInitialized() const {
  return camera_state_.IsInitialized();
}

bool CameraInterfaceBase::IsStarted() const {
  return camera_state_.IsStarted();
}

bool CameraInterfaceBase::IsStopped() const {
  return camera_state_.IsStopped();
}

}  // namespace felicia