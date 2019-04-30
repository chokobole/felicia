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
#include "third_party/chromium/base/memory/weak_ptr.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {

class V4l2Camera : public CameraInterface,
                   public ::base::SupportsWeakPtr<V4l2Camera> {
 public:
  ~V4l2Camera();

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

  V4l2Camera(const CameraDescriptor& camera_descriptor);

  static Status InitDevice(const CameraDescriptor& camera_descriptor, int* fd);
  Status InitMmap();
  void DoTakePhoto();

  static int DoIoctl(int fd, int request, void* argp);
  static bool RunIoctl(int fd, int request, void* argp);

  static std::vector<float> GetFrameRateList(int fd, uint32_t fourcc,
                                             uint32_t width, uint32_t height);

  CameraDescriptor camera_descriptor_;
  CameraFormat camera_format_;
  int fd_ = ::base::kInvalidPlatformFile;

  std::vector<CameraBuffer> buffers_;
  ::base::Thread thread_;

  ::base::TimeTicks first_ref_time_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(V4l2Camera);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_LINUX_V4L2_CAMERA_H_