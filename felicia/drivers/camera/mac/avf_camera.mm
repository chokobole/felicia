// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/mac/video_capture_device_mac.mm

#include "felicia/drivers/camera/mac/avf_camera.h"

#include "third_party/chromium/base/strings/sys_string_conversions.h"
#include "third_party/chromium/base/threading/thread_task_runner_handle.h"

#include "felicia/drivers/camera/camera_errors.h"
#import "felicia/drivers/camera/mac/avf_camera_delegate.h"
#include "felicia/drivers/camera/timestamp_constants.h"

namespace felicia {
namespace drivers {

// In device identifiers, the USB VID and PID are stored in 4 bytes each.
const size_t kVidPidSize = 4;

AvfCamera::AvfCamera(const CameraDescriptor& camera_descriptor)
    : CameraInterface(camera_descriptor),
      task_runner_(base::ThreadTaskRunnerHandle::Get()),
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

  if (!capture_device_) return felicia::errors::Unavailable("Failed to init capture device.");

  NSString* deviceId = [NSString stringWithUTF8String:camera_descriptor_.device_id().c_str()];
  NSString* errorMessage = nil;
  if (![capture_device_ setCaptureDevice:deviceId errorMessage:&errorMessage]) {
    return felicia::errors::Unavailable(base::StringPrintf(
        "Failed to set capture device: %s.", base::SysNSStringToUTF8(errorMessage).c_str()));
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
  requested_pixel_format_ = requested_camera_format.pixel_format();

  if (![capture_device_ startCapture]) {
    return felicia::errors::Unavailable("Failed to start capture.");
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
    LOG(ERROR) << base::SysNSStringToUTF8(errorMessage);

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
                             int aspect_denominator, base::TimeDelta timestamp) {
  if (camera_format_.pixel_format() != PixelFormat::PIXEL_FORMAT_MJPEG &&
      camera_format_.AllocationSize() != video_frame_length) {
    status_callback_.Run(errors::InvalidNumberOfBytesInBuffer());
    return;
  }

  if (requested_pixel_format_ == camera_format_.pixel_format()) {
    Data data(video_frame, video_frame_length);
    camera_frame_callback_.Run(CameraFrame{std::move(data), camera_format_, timestamp});
  } else {
    base::Optional<CameraFrame> camera_frame = ConvertToRequestedPixelFormat(
        video_frame, video_frame_length, camera_format_, requested_pixel_format_, timestamp);
    if (camera_frame.has_value()) {
      camera_frame_callback_.Run(std::move(camera_frame.value()));
    } else {
      status_callback_.Run(errors::FailedToConvertToRequestedPixelFormat(requested_pixel_format_));
    }
  }
}

void AvfCamera::ReceiveError(Status status) { status_callback_.Run(std::move(status)); }

// static
std::string AvfCamera::GetDeviceModelId(const std::string& device_id) {
  // The last characters of device id is a concatenation of VID and then PID.
  const size_t vid_location = device_id.size() - 2 * kVidPidSize;
  std::string id_vendor = device_id.substr(vid_location, kVidPidSize);
  const size_t pid_location = device_id.size() - kVidPidSize;
  std::string id_product = device_id.substr(pid_location, kVidPidSize);

  return id_vendor + ":" + id_product;
}

}  // namespace drivers
}  // namespace felicia