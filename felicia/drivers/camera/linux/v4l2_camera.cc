// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/linux/v4l2_capture_delegate.cc
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/linux/video_capture_device_factory_linux.cc

#include "felicia/drivers/camera/linux/v4l2_camera.h"

#include <linux/version.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/files/file_enumerator.h"
#include "third_party/chromium/base/files/file_util.h"
#include "third_party/chromium/base/files/scoped_file.h"
#include "third_party/chromium/base/posix/eintr_wrapper.h"
#include "third_party/chromium/base/stl_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/drivers/camera/camera_errors.h"

namespace felicia {

namespace {

constexpr size_t kDefaultWidth = 640;
constexpr size_t kDefaultHeight = 480;

// Maximum number of ioctl retries before giving up trying to reset controls.
constexpr int kMaxIOCtrlRetries = 5;

constexpr uint32_t kNumVideoBuffers = 4;

void FillV4L2Format(v4l2_format* format, uint32_t width, uint32_t height,
                    uint32_t pixelformat_fourcc) {
  memset(format, 0, sizeof(*format));
  format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format->fmt.pix.width = width;
  format->fmt.pix.height = height;
  format->fmt.pix.pixelformat = pixelformat_fourcc;
}

void FillV4L2Buffer(v4l2_buffer* buffer, int index) {
  memset(buffer, 0, sizeof(*buffer));
  buffer->memory = V4L2_MEMORY_MMAP;
  buffer->index = index;
  buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
}

void FillV4L2RequestBuffer(v4l2_requestbuffers* requestbuffers, int count) {
  memset(requestbuffers, 0, sizeof(*requestbuffers));
  requestbuffers->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  requestbuffers->memory = V4L2_MEMORY_MMAP;
  requestbuffers->count = count;
}

// USB VID and PID are both 4 bytes long.
const size_t kVidPidSize = 4;
const size_t kMaxInterfaceNameSize = 256;

// /sys/class/video4linux/video{N}/device is a symlink to the corresponding
// USB device info directory.
const char kVidPathTemplate[] = "/sys/class/video4linux/%s/device/../idVendor";
const char kPidPathTemplate[] = "/sys/class/video4linux/%s/device/../idProduct";
const char kInterfacePathTemplate[] =
    "/sys/class/video4linux/%s/device/interface";

bool ReadIdFile(const std::string& path, std::string* id) {
  char id_buf[kVidPidSize];
  FILE* file = fopen(path.c_str(), "rb");
  if (!file) return false;
  const bool success = fread(id_buf, kVidPidSize, 1, file) == 1;
  fclose(file);
  if (!success) return false;
  id->append(id_buf, kVidPidSize);
  return true;
}

std::string ExtractFileNameFromDeviceId(const std::string& device_id) {
  // |unique_id| is of the form "/dev/video2".  |file_name| is "video2".
  const char kDevDir[] = "/dev/";
  DCHECK(base::StartsWith(device_id, kDevDir, base::CompareCase::SENSITIVE));
  return device_id.substr(strlen(kDevDir), device_id.length());
}

class DevVideoFilePathsDeviceProvider {
 public:
  void GetDeviceIds(std::vector<std::string>* target_container) {
    const ::base::FilePath path("/dev/");
    ::base::FileEnumerator enumerator(path, false, base::FileEnumerator::FILES,
                                      "video*");
    while (!enumerator.Next().empty()) {
      const ::base::FileEnumerator::FileInfo info = enumerator.GetInfo();
      target_container->emplace_back(path.value() + info.GetName().value());
    }
  }

  std::string GetDeviceModelId(const std::string& device_id) {
    const std::string file_name = ExtractFileNameFromDeviceId(device_id);
    std::string usb_id;
    const std::string vid_path =
        ::base::StringPrintf(kVidPathTemplate, file_name.c_str());
    if (!ReadIdFile(vid_path, &usb_id)) return usb_id;

    usb_id.append(":");
    const std::string pid_path =
        base::StringPrintf(kPidPathTemplate, file_name.c_str());
    if (!ReadIdFile(pid_path, &usb_id)) usb_id.clear();

    return usb_id;
  }

  std::string GetDeviceDisplayName(const std::string& device_id) {
    const std::string file_name = ExtractFileNameFromDeviceId(device_id);
    const std::string interface_path =
        ::base::StringPrintf(kInterfacePathTemplate, file_name.c_str());
    std::string display_name;
    if (!::base::ReadFileToStringWithMaxSize(::base::FilePath(interface_path),
                                             &display_name,
                                             kMaxInterfaceNameSize)) {
      return std::string();
    }
    return display_name;
  }
};

}  // namespace

V4l2Camera::V4l2Camera(const CameraDescriptor& descriptor)
    : descriptor_(descriptor), thread_("V4l2CameraThread") {}

V4l2Camera::~V4l2Camera() {
  if (thread_.IsRunning()) thread_.Stop();
}

// static
Status V4l2Camera::GetCameraDescriptors(CameraDescriptors* camera_descriptors) {
  DCHECK(camera_descriptors);

  DevVideoFilePathsDeviceProvider device_provider;
  std::vector<std::string> filepaths;
  device_provider.GetDeviceIds(&filepaths);
  for (auto& unique_id : filepaths) {
    ::base::ScopedFD fd(HANDLE_EINTR(open(unique_id.c_str(), O_RDONLY)));
    if (!fd.is_valid()) {
      DLOG(ERROR) << "Couldn't open " << unique_id;
      continue;
    }

    v4l2_capability cap;
    if (!(DoIoctl(fd.get(), VIDIOC_QUERYCAP, &cap) == 0) &&
        (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
        !(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)) {
      continue;
    }

    std::string display_name = device_provider.GetDeviceDisplayName(unique_id);
    const std::string model_id = device_provider.GetDeviceModelId(unique_id);
    if (display_name.empty()) display_name = reinterpret_cast<char*>(cap.card);
    camera_descriptors->emplace_back(display_name, unique_id, model_id);
  }

  return Status::OK();
}

// static
Status V4l2Camera::GetSupportedCameraFormats(
    const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats) {
  return errors::Unimplemented("Not implemented yet.");
}

Status V4l2Camera::Init() {
  Status s = InitDevice();
  if (!s.ok()) {
    return s;
  }

  StatusOr<CameraFormat> status_or = GetCurrentCameraFormat();
  if (!status_or.ok()) {
    return status_or.status();
  }
  camera_format_ = status_or.ValueOrDie();
  DLOG(INFO) << "Default Format: " << camera_format_.ToString();

  return InitMmap();
}

Status V4l2Camera::Start(CameraFrameCallback camera_frame_callback,
                         StatusCallback status_callback) {
  for (size_t i = 0; i < buffers_.size(); ++i) {
    v4l2_buffer buffer;
    FillV4L2Buffer(&buffer, i);

    if (DoIoctl(fd_, VIDIOC_QBUF, &buffer) < 0) {
      return errors::FailedToEnqueueBuffer();
    }
  }

  v4l2_buf_type capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(fd_, VIDIOC_STREAMON, &capture_type) < 0) {
    return errors::FailedToStream();
  }

  thread_.Start();
  camera_frame_callback_ = camera_frame_callback;
  status_callback_ = status_callback;
  if (thread_.task_runner()->BelongsToCurrentThread()) {
    DoTakePhoto();
  } else {
    thread_.task_runner()->PostTask(
        FROM_HERE, ::base::BindOnce(&V4l2Camera::DoTakePhoto, AsWeakPtr()));
  }

  return Status::OK();
}

Status V4l2Camera::Close() {
  if (fd_ != ::base::kInvalidPlatformFile) close(fd_);
  return Status::OK();
}

StatusOr<CameraFormat> V4l2Camera::GetCurrentCameraFormat() {
  struct v4l2_format format;
  format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (DoIoctl(fd_, VIDIOC_G_FMT, &format) < 0) {
    return errors::FailedToGetCameraFormat();
  }

  return CameraFormat(
      format.fmt.pix.width, format.fmt.pix.height,
      CameraFormat::FromV4l2PixelFormat(format.fmt.pix.pixelformat));
}

Status V4l2Camera::SetCameraFormat(const CameraFormat& camera_format) {
  struct v4l2_format format;
  FillV4L2Format(&format, camera_format.width(), camera_format.height(),
                 camera_format.ToV4l2PixelFormat());
  if (DoIoctl(fd_, VIDIOC_S_FMT, &format) < 0) {
    return errors::FailedToSetFormat(camera_format);
  }
  camera_format_ = camera_format;
  return Status::OK();
}

Status V4l2Camera::InitDevice() {
  const std::string& device_id = descriptor_.device_id();
  fd_ = HANDLE_EINTR(open(device_id.c_str(), O_RDWR));
  if (fd_ == ::base::kInvalidPlatformFile)
    return errors::FailedToOpenCamera(device_id);

  v4l2_capability cap;
  if (!(DoIoctl(fd_, VIDIOC_QUERYCAP, &cap) == 0) &&
      (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
      !(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)) {
    return errors::NotAV4l2Device(device_id);
  }

  return Status::OK();
}

Status V4l2Camera::InitMmap() {
  v4l2_requestbuffers requestbuffers;
  FillV4L2RequestBuffer(&requestbuffers, kNumVideoBuffers);

  if (DoIoctl(fd_, VIDIOC_REQBUFS, &requestbuffers) < 0) {
    return errors::FailedToRequestMmapBuffers();
  }

  for (unsigned int i = 0; i < requestbuffers.count; ++i) {
    v4l2_buffer buffer;
    FillV4L2Buffer(&buffer, i);

    if (DoIoctl(fd_, VIDIOC_QUERYBUF, &buffer) < 0) {
      return errors::FailedToRequestMmapBuffers();
    }

    void* const start = mmap(nullptr, buffer.length, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd_, buffer.m.offset);
    if (start == MAP_FAILED) {
      return errors::FailedToMmapBuffers();
    }
    buffers_.emplace_back(static_cast<uint8_t*>(start), buffer.length);
  }

  return Status::OK();
}

void V4l2Camera::DoTakePhoto() {
  DCHECK(thread_.task_runner()->BelongsToCurrentThread());

  v4l2_buffer buffer;
  FillV4L2Buffer(&buffer, 0);
  if (DoIoctl(fd_, VIDIOC_DQBUF, &buffer) < 0) {
    status_callback_.Run(errors::FailedToDequeueBuffer());
    return;
  }

#ifdef V4L2_BUF_FLAG_ERROR
  bool buf_error_flag_set = buffer.flags & V4L2_BUF_FLAG_ERROR;
#else
  bool buf_error_flag_set = false;
#endif
  if (buf_error_flag_set) {
    status_callback_.Run(errors::V4l2ErrorFlagWasSet());
  } else {
    buffers_[buffer.index].set_payload(buffer.bytesused);
    ::base::Optional<CameraFrame> argb_frame =
        ConvertToARGB(buffers_[buffer.index], camera_format_);
    if (argb_frame.has_value()) {
      const base::TimeTicks now = base::TimeTicks::Now();
      if (first_ref_time_.is_null()) first_ref_time_ = now;
      const base::TimeDelta timestamp = now - first_ref_time_;
      argb_frame.value().set_timestamp(timestamp);
      camera_frame_callback_.Run(std::move(argb_frame.value()));
    } else {
      status_callback_.Run(errors::FailedToConvertToARGB());
    }
  }

  if (DoIoctl(fd_, VIDIOC_QBUF, &buffer) < 0) {
    status_callback_.Run(errors::FailedToEnqueueBuffer());
    return;
  }

  thread_.task_runner()->PostTask(
      FROM_HERE, ::base::BindOnce(&V4l2Camera::DoTakePhoto, AsWeakPtr()));
}

// static
int V4l2Camera::DoIoctl(int fd, int request, void* argp) {
  int ret = HANDLE_EINTR(ioctl(fd, request, argp));
  DPLOG_IF(ERROR, ret < 0) << "ioctl";
  return ret;
}

// static
bool V4l2Camera::RunIoctl(int fd, int request, void* argp) {
  int num_retries = 0;
  for (; DoIoctl(fd, request, argp) < 0 && num_retries < kMaxIOCtrlRetries;
       ++num_retries) {
    DPLOG(WARNING) << "ioctl";
  }
  DPLOG_IF(ERROR, num_retries == kMaxIOCtrlRetries);
  return num_retries != kMaxIOCtrlRetries;
}

}  // namespace felicia