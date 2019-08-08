// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/linux/v4l2_capture_delegate.cc

#ifndef FELICIA_DRIVERS_CAMERA_LINUX_V4L2_CAMERA_H_
#define FELICIA_DRIVERS_CAMERA_LINUX_V4L2_CAMERA_H_

#include <linux/videodev2.h>

#include "third_party/chromium/base/files/platform_file.h"
#include "third_party/chromium/base/files/scoped_file.h"
#include "third_party/chromium/base/memory/weak_ptr.h"
#include "third_party/chromium/base/synchronization/waitable_event.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/core/util/timestamp/timestamper.h"
#include "felicia/drivers/camera/camera_buffer.h"
#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {

class V4l2Camera : public CameraInterface,
                   public base::SupportsWeakPtr<V4l2Camera> {
 public:
  ~V4l2Camera();

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

  Status SetCameraSettings(const CameraSettings& camera_settings) override;
  Status GetCameraSettingsInfo(
      CameraSettingsInfoMessage* camera_settings) override;

 private:
  friend class CameraFactory;

  V4l2Camera(const CameraDescriptor& camera_descriptor);

  Status InitMmap();
  Status ClearMmap();
  Status SetCameraFormat(const CameraFormat& camera_format);
  void DoStop(base::WaitableEvent* event, Status* status);
  void DoCapture();

  void GetCameraSetting(int control_id, CameraSettingsModeValue* value);
  void GetCameraSetting(int control_id, CameraSettingsRangedValue* value);

  static int DoIoctl(int fd, int request, void* argp);
  static bool RunIoctl(int fd, int request, void* argp);

  static Status InitDevice(const CameraDescriptor& camera_descriptor,
                           base::ScopedFD* fd);
  static std::vector<float> GetFrameRateList(int fd, uint32_t fourcc,
                                             uint32_t width, uint32_t height);

  base::ScopedFD fd_;

  std::vector<CameraBuffer> buffers_;
  base::Thread thread_;

  Timestamper timestamper_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(V4l2Camera);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_LINUX_V4L2_CAMERA_H_