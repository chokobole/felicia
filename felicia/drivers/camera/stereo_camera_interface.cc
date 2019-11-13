// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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