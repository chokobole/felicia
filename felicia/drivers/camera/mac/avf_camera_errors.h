#ifndef FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_ERRORS_H_
#define FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_ERRORS_H_

#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/base/strings/sys_string_conversions.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {
namespace errors {

inline ::felicia::Status FailedToInit() { return Unavailable("Failed to init capture device."); }

inline ::felicia::Status FailedToSetCaptureDevice(NSString* errorMessage) {
  return Unavailable(::base::StringPrintf("Failed to set capture device: %s.",
                                          ::base::SysNSStringToUTF8(errorMessage).c_str()));
}

inline ::felicia::Status FailedtoStartCapture() { return Unavailable("Failed to start capture."); }

}  // namespace errors
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_MAC_AVF_CAMERA_ERRORS_H_