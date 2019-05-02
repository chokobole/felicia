// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/video_capture_device_mf_win.h

#ifndef FELICIA_DRIVERS_CAMERA_WIN_MF_CAMERA_H_
#define FELICIA_DRIVERS_CAMERA_WIN_MF_CAMERA_H_

#include "felicia/drivers/camera/camera_interface.h"

#include <mfidl.h>

namespace felicia {

class MfCamera : public CameraInterface {
 public:
  ~MfCamera();

  // Needed by CameraFactory
  static bool PlatformSupportsMediaFoundation();
  static Status GetCameraDescriptors(CameraDescriptors* camera_descriptors);
  static Status GetSupportedCameraFormats(
      const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats);

  // CameraInterface methods
  Status Init() override;
  Status Start(CameraFrameCallback camera_frame_callback,
               StatusCallback status_callback) override;
  Status Stop() override;

  StatusOr<CameraFormat> GetCurrentCameraFormat() override;
  Status SetCameraFormat(const CameraFormat& format) override;

 private:
  friend class CameraFactory;

  MfCamera(const CameraDescriptor& camera_descriptor);

  DISALLOW_IMPLICIT_CONSTRUCTORS(MfCamera);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_MF_CAMERA_H_