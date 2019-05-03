#ifndef FELICIA_DRIVERS_CAMERA_LINUX_V4L2_CAMERA_ERRORS_H_
#define FELICIA_DRIVERS_CAMERA_LINUX_V4L2_CAMERA_ERRORS_H_

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {
namespace errors {

inline ::felicia::Status FailedToOpenCamera(const std::string& device_id) {
  return Unavailable(
      ::base::StringPrintf("Failed to open %s.", device_id.c_str()));
}

inline ::felicia::Status FailedToRequestMmapBuffers() {
  return Unavailable("Failed to request mmap buffers from V4L2.");
}

inline ::felicia::Status FailedToMmapBuffers() {
  return Unavailable("Failed to mmap buffers.");
}

inline ::felicia::Status FailedToEnqueueBuffer() {
  return Unavailable("Failed to enqueue V4L2 buffer to the driver.");
}

inline ::felicia::Status FailedToDequeueBuffer() {
  return Unavailable("Failed to dequeue V4L2 buffer from the driver.");
}

inline ::felicia::Status FailedToStreamOn() {
  return Unavailable("Failed to stream on.");
}

inline ::felicia::Status FailedToStreamOff() {
  return Unavailable("Failed to stream off.");
}

inline ::felicia::Status V4l2ErrorFlagWasSet() {
  return Unavailable("V4l2 Error flag was set.");
}

}  // namespace errors
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_LINUX_V4L2_CAMERA_ERRORS_H_