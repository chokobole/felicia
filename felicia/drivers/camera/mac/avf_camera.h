// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/mac/video_capture_device_mac.h

#ifndef FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_H_
#define FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_H_

#include "third_party/chromium/base/mac/scoped_nsobject.h"
#include "third_party/chromium/base/memory/ref_counted.h"
#include "third_party/chromium/base/single_thread_task_runner.h"

#include "felicia/drivers/camera/camera_interface.h"
#include "felicia/drivers/camera/mac/avf_camera_delegate.h"
#include "felicia/drivers/camera/mac/frame_receiver.h"

namespace felicia {

class AvfCamera : public CameraInterface, public FrameReceiver {
 public:
  ~AvfCamera();

  // Needed by CameraFactory
  static Status GetCameraDescriptors(CameraDescriptors* camera_descriptors);
  static Status GetSupportedCameraFormats(
      const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats);

  // CameraInterface methods
  Status Init() override;
  Status Start(const CameraFormat& requested_camera_format,
               CameraFrameCallback camera_frame_callback,
               StatusCallback status_callback) override;
  Status Stop() override;

  // FrameReceiver methods
  void ReceiveFrame(const uint8_t* video_frame, int video_frame_length,
                    const CameraFormat& camera_format, int aspect_numerator,
                    int aspect_denominator,
                    ::base::TimeDelta timestamp) override;

  void ReceiveError(const Status& status) override;

 private:
  friend class CameraFactory;

  AvfCamera(const CameraDescriptor& camera_descriptor);

  Status SetCameraFormat(const CameraFormat& camera_format);

  static std::string GetDeviceModelId(const std::string& device_id);

  const scoped_refptr<::base::SingleThreadTaskRunner> task_runner_;

  ::base::scoped_nsobject<AvfCameraDelegate> capture_device_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(AvfCamera);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_H_