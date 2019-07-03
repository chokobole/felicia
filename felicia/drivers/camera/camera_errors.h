#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_ERRORS_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_ERRORS_H_

#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/drivers/camera/camera_format.h"

namespace felicia {
namespace errors {

inline ::felicia::Status NoVideoCapbility() {
  return NotFound("No video capability.");
}

inline ::felicia::Status FailedToGetCameraDescriptors() {
  return Unavailable("Failed to get camera descriptors.");
}

inline ::felicia::Status FailedToGetFrameRate() {
  return Unavailable("Failed to get frame rate.");
}

inline ::felicia::Status FailedToSetCameraFormat() {
  return Unavailable("Failed to set camera format.");
}

inline ::felicia::Status FailedToSetFrameRate() {
  return Unavailable("Failed to set frame rate.");
}

inline ::felicia::Status InvalidNumberOfBytesInBuffer() {
  return InvalidArgument("Invalid number of bytes in buffer.");
}

inline ::felicia::Status FailedToConvertToRequestedPixelFormat(
    PixelFormat requested_pixel_format) {
  return Unavailable(
      ::base::StringPrintf("Failed To convert to %s.",
                           PixelFormat_Name(requested_pixel_format).c_str()));
}

}  // namespace errors
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_ERRORS_H_