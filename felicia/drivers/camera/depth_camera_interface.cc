// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/drivers/camera/depth_camera_interface.h"

namespace felicia {
namespace drivers {

DepthCameraInterface::DepthCameraInterface(
    const CameraDescriptor& camera_descriptor)
    : CameraInterfaceBase(camera_descriptor) {}

const CameraFormat& DepthCameraInterface::color_format() const {
  return color_format_;
}

const CameraFormat& DepthCameraInterface::depth_format() const {
  return depth_format_;
}

}  // namespace drivers
}  // namespace felicia