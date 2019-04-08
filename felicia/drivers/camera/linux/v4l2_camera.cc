#include "felicia/drivers/camera/linux/v4l2_camera.h"

#include <linux/version.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/posix/eintr_wrapper.h"
#include "third_party/chromium/base/stl_util.h"

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

}  // namespace

V4l2Camera::V4l2Camera(const CameraDescriptor& descriptor)
    : descriptor_(descriptor), thread_("V4l2CameraThread") {}

V4l2Camera::~V4l2Camera() {
  if (thread_.IsRunning()) thread_.Stop();
}

Status V4l2Camera::Init() {
  Status s = InitDevice();
  if (!s.ok()) {
    return s;
  }

  StatusOr<CameraFormat> status_or = GetFormat();
  if (!status_or.ok()) {
    return status_or.status();
  }
  camera_format_ = status_or.ValueOrDie();
  LOG(INFO) << "Default Format: " << camera_format_.ToString();

  return InitMmap();
}

Status V4l2Camera::Start(CameraFrameCallback callback) {
  for (size_t i = 0; i < buffers_.size(); ++i) {
    v4l2_buffer buffer;
    FillV4L2Buffer(&buffer, i);

    if (DoIoctl(VIDIOC_QBUF, &buffer) < 0) {
      return errors::FailedToEnqueueBuffer();
    }
  }

  v4l2_buf_type capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(VIDIOC_STREAMON, &capture_type) < 0) {
    return errors::FailedToStream();
  }

  thread_.Start();
  callback_ = callback;
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

StatusOr<CameraFormat> V4l2Camera::GetFormat() {
  struct v4l2_format format;
  format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (DoIoctl(VIDIOC_G_FMT, &format) < 0) {
    return errors::FailedToGetFormat();
  }

  return CameraFormat(
      format.fmt.pix.width, format.fmt.pix.height,
      CameraFormat::FromV4l2PixelFormat(format.fmt.pix.pixelformat));
}

Status V4l2Camera::SetFormat(CameraFormat camera_format) {
  struct v4l2_format format;
  FillV4L2Format(&format, camera_format.width(), camera_format.height(),
                 camera_format.ToV4l2PixelFormat());
  if (DoIoctl(VIDIOC_S_FMT, &format) < 0) {
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
  if (DoIoctl(VIDIOC_QUERYCAP, &cap) < 0) {
    return errors::NotAV4l2Device(device_id);
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    return errors::NoCapability(device_id, "Video Capture Interface");
  }

  return Status::OK();
}

Status V4l2Camera::InitMmap() {
  v4l2_requestbuffers requestbuffers;
  FillV4L2RequestBuffer(&requestbuffers, kNumVideoBuffers);

  if (DoIoctl(VIDIOC_REQBUFS, &requestbuffers) < 0) {
    return errors::FailedToRequestMmapBuffers();
  }

  for (unsigned int i = 0; i < requestbuffers.count; ++i) {
    v4l2_buffer buffer;
    FillV4L2Buffer(&buffer, i);

    if (DoIoctl(VIDIOC_QUERYBUF, &buffer) < 0) {
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
  if (DoIoctl(VIDIOC_DQBUF, &buffer) < 0) {
    callback_.Run(errors::FailedToDequeueBuffer());
    return;
  }

#ifdef V4L2_BUF_FLAG_ERROR
  bool buf_error_flag_set = buffer.flags & V4L2_BUF_FLAG_ERROR;
#else
  bool buf_error_flag_set = false;
#endif
  if (buf_error_flag_set) {
    callback_.Run(errors::V4l2ErrorFlagWasSet());
  } else {
    buffers_[buffer.index].set_payload(buffer.bytesused);
    ::base::Optional<CameraFrame> argb_frame =
        ConvertToARGB(buffers_[buffer.index], camera_format_);
    if (argb_frame.has_value()) {
      argb_frame.value().set_timestamp(
          ::base::Time::FromTimeVal(buffer.timestamp));
      callback_.Run(argb_frame.value());
    } else {
      callback_.Run(errors::FailedToConvertToARGB());
    }
  }

  if (DoIoctl(VIDIOC_QBUF, &buffer) < 0) {
    callback_.Run(errors::FailedToEnqueueBuffer());
    return;
  }

  thread_.task_runner()->PostTask(
      FROM_HERE, ::base::BindOnce(&V4l2Camera::DoTakePhoto, AsWeakPtr()));
}

int V4l2Camera::DoIoctl(int request, void* argp) {
  int ret = HANDLE_EINTR(ioctl(fd_, request, argp));
  DPLOG_IF(ERROR, ret < 0) << "ioctl";
  return ret;
}

bool V4l2Camera::RunIoctl(int request, void* argp) {
  int num_retries = 0;
  for (; DoIoctl(request, argp) < 0 && num_retries < kMaxIOCtrlRetries;
       ++num_retries) {
    DPLOG(WARNING) << "ioctl";
  }
  DPLOG_IF(ERROR, num_retries == kMaxIOCtrlRetries);
  return num_retries != kMaxIOCtrlRetries;
}

}  // namespace felicia