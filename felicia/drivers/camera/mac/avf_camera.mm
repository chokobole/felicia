// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/mac/video_capture_device_mac.mm

#include "felicia/drivers/camera/mac/avf_camera.h"

#include "third_party/chromium/base/threading/thread_task_runner_handle.h"

#include "felicia/drivers/camera/camera_errors.h"
#import "felicia/drivers/camera/mac/avf_camera_delegate.h"
#include "felicia/drivers/camera/timestamp_constants.h"

namespace felicia {

// In device identifiers, the USB VID and PID are stored in 4 bytes each.
const size_t kVidPidSize = 4;

AvfCamera::AvfCamera(const CameraDescriptor& camera_descriptor)
    : CameraInterface(camera_descriptor),
      task_runner_(::base::ThreadTaskRunnerHandle::Get()),
      capture_device_(nil) {}

AvfCamera::~AvfCamera() { DCHECK(task_runner_->BelongsToCurrentThread()); }

// static
Status AvfCamera::GetCameraDescriptors(CameraDescriptors* camera_descriptors) {
  DCHECK(camera_descriptors);

  NSDictionary* capture_devices;
  capture_devices = [AvfCameraDelegate deviceNames];
  for (NSString* key in capture_devices) {
    const std::string device_id = [key UTF8String];
    const std::string display_name = [[[capture_devices valueForKey:key] deviceName] UTF8String];
    const std::string model_id = AvfCamera::GetDeviceModelId(device_id);
    camera_descriptors->emplace_back(display_name, device_id, model_id);
  }
  return Status::OK();
}

// status
Status AvfCamera::GetSupportedCameraFormats(const CameraDescriptor& camera_descriptor,
                                            CameraFormats* camera_formats) {
  DCHECK(camera_formats);

  DVLOG(1) << "Enumerating video capture capabilities, AVFoundation";
  [AvfCameraDelegate getDevice:camera_descriptor supportedFormats:camera_formats];

  return Status::OK();
}

Status AvfCamera::Init() {
  if (!camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }

  DCHECK(task_runner_->BelongsToCurrentThread());

  capture_device_.reset([[AvfCameraDelegate alloc] initWithFrameReceiver:this]);

  if (!capture_device_) return errors::FailedToInit();

  NSString* deviceId = [NSString stringWithUTF8String:camera_descriptor_.device_id().c_str()];
  NSString* errorMessage = nil;
  if (![capture_device_ setCaptureDevice:deviceId errorMessage:&errorMessage]) {
    return errors::FailedToSetCaptureDevice(errorMessage);
  }

  camera_state_.ToInitialized();

  return Status::OK();
}

Status AvfCamera::Start(const CameraFormat& requested_camera_format,
                        CameraFrameCallback camera_frame_callback, StatusCallback status_callback) {
  DCHECK(task_runner_->BelongsToCurrentThread());

  if (!camera_state_.IsInitialized()) {
    return camera_state_.InvalidStateError();
  }

  CameraFormats camera_formats;
  Status s = GetSupportedCameraFormats(camera_descriptor_, &camera_formats);
  if (!s.ok()) return s;

  const CameraFormat& final_camera_format =
      GetBestMatchedCameraFormat(requested_camera_format, camera_formats);
  s = SetCameraFormat(final_camera_format);
  if (!s.ok()) return s;

  if (![capture_device_ startCapture]) {
    return errors::FailedtoStartCapture();
  }

  camera_frame_callback_ = camera_frame_callback;
  status_callback_ = status_callback;

  camera_state_.ToStarted();

  return Status::OK();
}

Status AvfCamera::Stop() {
  DCHECK(task_runner_->BelongsToCurrentThread());

  if (!camera_state_.IsStarted()) {
    return camera_state_.InvalidStateError();
  }

  NSString* errorMessage = nil;
  if (![capture_device_ setCaptureDevice:nil errorMessage:&errorMessage])
    LOG(ERROR) << ::base::SysNSStringToUTF8(errorMessage);

  [capture_device_ setFrameReceiver:nil];

  camera_frame_callback_.Reset();
  status_callback_.Reset();
  camera_state_.ToStopped();

  return Status::OK();
}

Status AvfCamera::SetCameraFormat(const CameraFormat& camera_format) {
  if (![capture_device_ setCaptureHeight:camera_format.height()
                                   width:camera_format.width()
                               frameRate:camera_format.frame_rate()
                                  fourcc:camera_format.ToAVFoundationPixelFormat()]) {
    return errors::FailedToSetCameraFormat();
  }

  camera_format_ = camera_format;
  DVLOG(0) << "Set CameraFormat to " << camera_format_.ToString();

  return Status::OK();
}

void AvfCamera::ReceiveFrame(const uint8_t* video_frame, int video_frame_length,
                             const CameraFormat& camera_format, int aspect_numerator,
                             int aspect_denominator, ::base::TimeDelta timestamp) {
  CameraBuffer camera_buffer(const_cast<uint8_t*>(video_frame), video_frame_length);
  camera_buffer.set_payload(video_frame_length);
  ::base::Optional<CameraFrame> argb_frame = ConvertToARGB(camera_buffer, camera_format);
  if (argb_frame.has_value()) {
    if (first_ref_time_.is_null()) first_ref_time_ = base::TimeTicks::Now();

    // There is a chance that the platform does not provide us with the
    // timestamp, in which case, we use reference time to calculate a timestamp.
    if (timestamp == kNoTimestamp) timestamp = base::TimeTicks::Now() - first_ref_time_;

    argb_frame.value().set_timestamp(timestamp);
    camera_frame_callback_.Run(std::move(argb_frame.value()));
  } else {
    status_callback_.Run(errors::FailedToConvertToARGB());
  }
}

void AvfCamera::ReceiveError(const Status& status) { status_callback_.Run(status); }

// static
std::string AvfCamera::GetDeviceModelId(const std::string& device_id) {
  // The last characters of device id is a concatenation of VID and then PID.
  const size_t vid_location = device_id.size() - 2 * kVidPidSize;
  std::string id_vendor = device_id.substr(vid_location, kVidPidSize);
  const size_t pid_location = device_id.size() - kVidPidSize;
  std::string id_product = device_id.substr(pid_location, kVidPidSize);

  return id_vendor + ":" + id_product;
}

}  // namespace felicia