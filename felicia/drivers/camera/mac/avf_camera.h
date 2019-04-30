// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/mac/video_capture_device_mac.h

#ifndef FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_H_
#define FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_H_

#include "third_party/chromium/base/memory/ref_counted.h"
#include "third_party/chromium/base/single_thread_task_runner.h"

#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {

class AvfCamera : public CameraInterface {
 public:
  ~AvfCamera();

  // Needed by CameraFactory
  static Status GetCameraDescriptors(CameraDescriptors* camera_descriptors);
  static Status GetSupportedCameraFormats(
      const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats);

  // CameraInterface methods
  Status Init() override;
  Status Start(CameraFrameCallback camera_frame_callback,
               StatusCallback status_callback) override;
  Status Close() override;

  StatusOr<CameraFormat> GetCurrentCameraFormat() override;
  Status SetCameraFormat(const CameraFormat& format) override;

 private:
  friend class CameraFactory;

  AvfCamera(const CameraDescriptor& camera_descriptor);

  static std::string GetDeviceModelId(const std::string& device_id);

  CameraDescriptor camera_descriptor_;
  CameraFormat camera_format_;

  const scoped_refptr<::base::SingleThreadTaskRunner> task_runner_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(AvfCamera);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_H_