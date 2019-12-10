// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_
#define FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_

#include "felicia/drivers/camera/camera_interface_base.h"
#include "felicia/drivers/camera/depth_camera_frame.h"

namespace felicia {
namespace drivers {

class FEL_EXPORT DepthCameraInterface : public CameraInterfaceBase {
 public:
  explicit DepthCameraInterface(const CameraDescriptor& camera_descriptor);

  virtual Status Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       CameraFrameCallback color_frame_callback,
                       DepthCameraFrameCallback depth_frame_callback,
                       StatusCallback status_callback) = 0;

  const CameraFormat& color_format() const;
  const CameraFormat& depth_format() const;

 protected:
  CameraFormat color_format_;
  CameraFormat depth_format_;
  PixelFormat requested_pixel_format_;

  CameraFrameCallback color_frame_callback_;
  DepthCameraFrameCallback depth_frame_callback_;
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_