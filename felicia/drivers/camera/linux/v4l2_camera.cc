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

constexpr uint32_t kNumVideoBuffers = 4;

void FillV4L2Buffer(v4l2_buffer* buffer, int index) {
  memset(buffer, 0, sizeof(*buffer));
  buffer->memory = V4L2_MEMORY_MMAP;
  buffer->index = index;
  buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
}

void FillV4L2RequestBuffer(v4l2_requestbuffers* request_buffer, int count) {
  memset(request_buffer, 0, sizeof(*request_buffer));
  request_buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  request_buffer->memory = V4L2_MEMORY_MMAP;
  request_buffer->count = count;
}

}  // namespace

V4l2Camera::V4l2Camera(const CameraDescriptor& descriptor)
    : descriptor_(descriptor), thread_("V4l2CameraThread") {}

V4l2Camera::~V4l2Camera() { thread_.Stop(); }

Status V4l2Camera::Init() {
  Status s = InitDevice();
  if (!s.ok()) {
    fd_ = ::base::kInvalidPlatformFile;
    return s;
  }

  return InitMmap();
}

Status V4l2Camera::Start() {
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
  if (thread_.task_runner()->BelongsToCurrentThread()) {
    DoTakePhoto();
  } else {
    thread_.task_runner()->PostTask(
        FROM_HERE, ::base::BindOnce(&V4l2Camera::DoTakePhoto, AsWeakPtr()));
  }

  return Status::OK();
}

Status V4l2Camera::Close() { return Status::OK(); }

Status V4l2Camera::TakePhoto() { return Status::OK(); }

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
  v4l2_requestbuffers request_buffers;
  FillV4L2RequestBuffer(&request_buffers, kNumVideoBuffers);

  if (DoIoctl(VIDIOC_REQBUFS, &request_buffers) < 0) {
    return errors::FailedToRequestMmapBuffers();
  }

  for (unsigned int i = 0; i < request_buffers.count; ++i) {
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
    error_status_ = errors::FailedToDequeueBuffer();
    return;
  }

#ifdef V4L2_BUF_FLAG_ERROR
  bool buf_error_flag_set = buffer.flags & V4L2_BUF_FLAG_ERROR;
#else
  bool buf_error_flag_set = false;
#endif
  if (buf_error_flag_set) {
#ifdef V4L2_BUF_FLAG_ERROR
    LOG(ERROR) << "V4l2 error flag was set.";
#endif
  } else {
    LOG(INFO) << "Take photo";
    buffers_[buffer.index].set_payload(buffer.bytesused);
  }

  if (DoIoctl(VIDIOC_QBUF, &buffer) < 0) {
    error_status_ = errors::FailedToEnqueueBuffer();
  }

  thread_.task_runner()->PostTask(
      FROM_HERE, ::base::BindOnce(&V4l2Camera::DoTakePhoto, AsWeakPtr()));
}

int V4l2Camera::DoIoctl(int request, void* argp) {
  return HANDLE_EINTR(ioctl(fd_, request, argp));
}

}  // namespace felicia