// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/drivers/camera/camera_interface_base.h"

namespace felicia {
namespace drivers {

CameraInterfaceBase::CameraInterfaceBase(
    const CameraDescriptor& camera_descriptor)
    : camera_descriptor_(camera_descriptor) {}

CameraInterfaceBase::~CameraInterfaceBase() = default;

Status CameraInterfaceBase::SetCameraSettings(
    const CameraSettings& camera_settings) {
  return felicia::errors::Unimplemented("Not implemented yet.");
}

Status CameraInterfaceBase::GetCameraSettingsInfo(
    CameraSettingsInfoMessage* camera_settings) {
  return felicia::errors::Unimplemented("Not implemented yet.");
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

}  // namespace drivers
}  // namespace felicia