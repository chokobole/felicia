// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/mac/video_capture_device_mac.mm

#include "felicia/drivers/camera/mac/avf_camera.h"

#include "third_party/chromium/base/threading/thread_task_runner_handle.h"

#import "felicia/drivers/camera/mac/avf_camera_delegate.h"
#include "felicia/drivers/camera/mac/avf_camera_errors.h"

namespace felicia {

AvfCamera::AvfCamera(const CameraDescriptor& descriptor)
    : descriptor_(descriptor), task_runner_(::base::ThreadTaskRunnerHandle::Get()) {}

AvfCamera::~AvfCamera() {}

// static
Status AvfCamera::GetCameraDescriptors(CameraDescriptors* camera_descriptors) {
  NSDictionary* capture_devices;
  capture_devices = [AvfCameraDelegate deviceNames];
  for (NSString* key in capture_devices) {
    const std::string device_id = [key UTF8String];
    const std::string display_name = [[[capture_devices valueForKey:key] deviceName] UTF8String];
    camera_descriptors->push_back({display_name, device_id});
  }
  return Status::OK();
}

Status AvfCamera::Init() {
  DCHECK(task_runner_->BelongsToCurrentThread());

  return Status::OK();
}

Status AvfCamera::Start(CameraFrameCallback camera_frame_callback, StatusCallback status_callback) {
  camera_frame_callback_ = camera_frame_callback;
  status_callback_ = status_callback;

  return Status::OK();
}

Status AvfCamera::Close() { return errors::Unimplemented("Not implemented yet."); }

Status AvfCamera::GetSupportedCameraFormats(CameraFormats* camera_formats) {
  return errors::Unimplemented("Not implemented yet.");
}

StatusOr<CameraFormat> AvfCamera::GetCurrentCameraFormat() {
  return errors::Unimplemented("Not implemented yet.");
}

Status AvfCamera::SetCameraFormat(const CameraFormat& camera_format) {
  camera_format_ = camera_format;
  return Status::OK();
}

}  // namespace felicia