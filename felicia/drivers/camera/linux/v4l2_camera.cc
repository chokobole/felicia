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

constexpr int kTypicalFramerate = 30;

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

V4l2Camera::V4l2Camera(const CameraDescriptor& camera_descriptor)
    : CameraInterface(camera_descriptor), thread_("V4l2CameraThread") {}

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
  DCHECK(camera_formats->empty());

  ::base::ScopedFD fd;
  Status s = InitDevice(camera_descriptor, &fd);
  if (!s.ok()) return s;

  v4l2_fmtdesc v4l2_format = {};
  v4l2_format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  for (; DoIoctl(fd.get(), VIDIOC_ENUM_FMT, &v4l2_format) == 0;
       ++v4l2_format.index) {
    CameraFormat camera_format;
    camera_format.set_pixel_format(
        CameraFormat::FromV4l2PixelFormat(v4l2_format.pixelformat));

    if (camera_format.pixel_format() == PIXEL_FORMAT_UNKNOWN) continue;

    v4l2_frmsizeenum frame_size = {};
    frame_size.pixel_format = v4l2_format.pixelformat;
    for (; DoIoctl(fd.get(), VIDIOC_ENUM_FRAMESIZES, &frame_size) == 0;
         ++frame_size.index) {
      if (frame_size.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
        camera_format.SetSize(frame_size.discrete.width,
                              frame_size.discrete.height);
      } else if (frame_size.type == V4L2_FRMSIZE_TYPE_STEPWISE ||
                 frame_size.type == V4L2_FRMSIZE_TYPE_CONTINUOUS) {
        // TODO(mcasas): see http://crbug.com/249953, support these devices.
        NOTIMPLEMENTED_LOG_ONCE();
      }

      const std::vector<float> frame_rates = GetFrameRateList(
          fd.get(), v4l2_format.pixelformat, frame_size.discrete.width,
          frame_size.discrete.height);
      for (const auto& frame_rate : frame_rates) {
        camera_format.set_frame_rate(frame_rate);
        camera_formats->push_back(camera_format);
        DVLOG(1) << camera_format.ToString();
      }
    }
  }

  return Status::OK();
}

Status V4l2Camera::Init() {
  if (!camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }

  Status s = InitDevice(camera_descriptor_, &fd_);
  if (!s.ok()) {
    return s;
  }

  camera_state_.ToInitialized();

  return Status::OK();
}

Status V4l2Camera::Start(const CameraFormat& requested_camera_format,
                         CameraFrameCallback camera_frame_callback,
                         StatusCallback status_callback) {
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

  s = InitMmap();
  if (!s.ok()) return s;

  for (size_t i = 0; i < buffers_.size(); ++i) {
    v4l2_buffer buffer;
    FillV4L2Buffer(&buffer, i);

    if (DoIoctl(fd_.get(), VIDIOC_QBUF, &buffer) < 0) {
      return errors::FailedToEnqueueBuffer();
    }
  }

  v4l2_buf_type capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(fd_.get(), VIDIOC_STREAMON, &capture_type) < 0) {
    return errors::FailedToStreamOn();
  }

  thread_.Start();
  camera_frame_callback_ = camera_frame_callback;
  status_callback_ = status_callback;
  camera_state_.ToStarted();

  if (thread_.task_runner()->BelongsToCurrentThread()) {
    DoCapture();
  } else {
    thread_.task_runner()->PostTask(
        FROM_HERE, ::base::BindOnce(&V4l2Camera::DoCapture, AsWeakPtr()));
  }

  return Status::OK();
}

Status V4l2Camera::Stop() {
  if (!camera_state_.IsStarted()) {
    return camera_state_.InvalidStateError();
  }

  Status s;
  if (thread_.task_runner()->BelongsToCurrentThread()) {
    DoStop(&s);
  } else {
    thread_.task_runner()->PostTask(
        FROM_HERE, ::base::BindOnce(&V4l2Camera::DoStop, AsWeakPtr(), &s));
  }

  if (thread_.IsRunning()) thread_.Stop();
  fd_.reset();

  camera_frame_callback_.Reset();
  status_callback_.Reset();
  camera_state_.ToStopped();

  return s;
}

Status V4l2Camera::InitMmap() {
  v4l2_requestbuffers requestbuffers;
  FillV4L2RequestBuffer(&requestbuffers, kNumVideoBuffers);

  if (DoIoctl(fd_.get(), VIDIOC_REQBUFS, &requestbuffers) < 0) {
    return errors::FailedToRequestMmapBuffers();
  }

  for (unsigned int i = 0; i < requestbuffers.count; ++i) {
    v4l2_buffer buffer;
    FillV4L2Buffer(&buffer, i);

    if (DoIoctl(fd_.get(), VIDIOC_QUERYBUF, &buffer) < 0) {
      return errors::FailedToRequestMmapBuffers();
    }

    void* const start = mmap(nullptr, buffer.length, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd_.get(), buffer.m.offset);
    if (start == MAP_FAILED) {
      return errors::FailedToMmapBuffers();
    }
    buffers_.emplace_back(static_cast<uint8_t*>(start), buffer.length);
  }

  return Status::OK();
}

Status V4l2Camera::ClearMmap() {
  for (auto& buffer : buffers_) {
    const int result = munmap(buffer.start(), buffer.length());
    PLOG_IF(ERROR, result < 0) << "Error munmap()ing V4L2 buffer";
  }

  buffers_.clear();

  return Status::OK();
}

Status V4l2Camera::SetCameraFormat(const CameraFormat& camera_format) {
  v4l2_format format;
  FillV4L2Format(&format, camera_format.width(), camera_format.height(),
                 camera_format.ToV4l2PixelFormat());
  if (DoIoctl(fd_.get(), VIDIOC_S_FMT, &format) < 0) {
    return errors::FailedToSetV4l2Format();
  }

  v4l2_streamparm streamparm = {};
  streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(fd_.get(), VIDIOC_G_PARM, &streamparm) < 0) {
    return errors::FailedToGetFrameRate();
  }

  if (streamparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) {
    streamparm.parm.capture.timeperframe.numerator = kFrameRatePrecision;
    streamparm.parm.capture.timeperframe.denominator =
        (camera_format.frame_rate())
            ? (camera_format.frame_rate() * kFrameRatePrecision)
            : (kTypicalFramerate * kFrameRatePrecision);

    if (DoIoctl(fd_.get(), VIDIOC_S_PARM, &streamparm) < 0) {
      return errors::FailedToSetFrameRate();
    }
  } else {
    DVLOG(2) << "No capability to set frame rate";
  }

  camera_format_ = camera_format;
  DVLOG(0) << "Set CameraFormat to " << camera_format_.ToString();

  return Status::OK();
}

void V4l2Camera::DoStop(Status* status) {
  DCHECK(thread_.task_runner()->BelongsToCurrentThread());

  v4l2_buf_type capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(fd_.get(), VIDIOC_STREAMOFF, &capture_type) < 0) {
    *status = errors::FailedToStreamOff();
    return;
  }

  Status s = ClearMmap();
  if (!s.ok()) {
    *status = s;
    return;
  }

  v4l2_requestbuffers requestbuffers;
  FillV4L2RequestBuffer(&requestbuffers, 0);
  if (DoIoctl(fd_.get(), VIDIOC_REQBUFS, &requestbuffers) < 0) {
    *status = errors::FailedToRequestMmapBuffers();
    return;
  }

  *status = Status::OK();
}

void V4l2Camera::DoCapture() {
  DCHECK(thread_.task_runner()->BelongsToCurrentThread());

  v4l2_buffer buffer;
  FillV4L2Buffer(&buffer, 0);
  if (DoIoctl(fd_.get(), VIDIOC_DQBUF, &buffer) < 0) {
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
  } else if (buffer.bytesused != camera_format_.AllocationSize()) {
    buffer.bytesused = 0;
    status_callback_.Run(errors::InvalidNumberOfBytesInBuffer());
  } else {
    CameraBuffer& camera_buffer = buffers_[buffer.index];
    camera_buffer.set_payload(buffer.bytesused);
    std::unique_ptr<uint8_t[]> data(new uint8_t[camera_buffer.payload()]);
    memcpy(data.get(), camera_buffer.start(), camera_buffer.payload());
    CameraFrame camera_frame(std::move(data), camera_format_);
    ;
    camera_frame_callback_.Run(std::move(camera_frame));
  }

  if (DoIoctl(fd_.get(), VIDIOC_QBUF, &buffer) < 0) {
    status_callback_.Run(errors::FailedToEnqueueBuffer());
    return;
  }

  thread_.task_runner()->PostTask(
      FROM_HERE, ::base::BindOnce(&V4l2Camera::DoCapture, AsWeakPtr()));
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

// static
Status V4l2Camera::InitDevice(const CameraDescriptor& camera_descriptor,
                              ::base::ScopedFD* fd) {
  const std::string& device_id = camera_descriptor.device_id();
  ::base::ScopedFD fd_temp(HANDLE_EINTR(open(device_id.c_str(), O_RDWR)));
  if (fd_temp == ::base::kInvalidPlatformFile)
    return errors::FailedToOpenCamera(device_id);

  v4l2_capability cap;
  if (!(DoIoctl(fd_temp.get(), VIDIOC_QUERYCAP, &cap) == 0) &&
      (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
      !(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)) {
    return errors::NoVideoCapbility();
  }

  fd->swap(fd_temp);
  return Status::OK();
}

// static
std::vector<float> V4l2Camera::GetFrameRateList(int fd, uint32_t fourcc,
                                                uint32_t width,
                                                uint32_t height) {
  std::vector<float> frame_rates;

  v4l2_frmivalenum frame_interval = {};
  frame_interval.pixel_format = fourcc;
  frame_interval.width = width;
  frame_interval.height = height;
  for (; DoIoctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frame_interval) == 0;
       ++frame_interval.index) {
    if (frame_interval.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
      if (frame_interval.discrete.numerator != 0) {
        frame_rates.push_back(
            frame_interval.discrete.denominator /
            static_cast<float>(frame_interval.discrete.numerator));
      }
    } else if (frame_interval.type == V4L2_FRMIVAL_TYPE_CONTINUOUS ||
               frame_interval.type == V4L2_FRMIVAL_TYPE_STEPWISE) {
      // TODO(mcasas): see http://crbug.com/249953, support these devices.
      NOTIMPLEMENTED_LOG_ONCE();
      break;
    }
  }
  // Some devices, e.g. Kinect, do not enumerate any frame rates, see
  // http://crbug.com/412284. Set their frame_rate to zero.
  if (frame_rates.empty()) frame_rates.push_back(0);
  return frame_rates;
}

}  // namespace felicia