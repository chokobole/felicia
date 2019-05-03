#ifndef FELICIA_DRIVERS_CAMERA_WIN_MF_CAMERA_ERRORS_H_
#define FELICIA_DRIVERS_CAMERA_WIN_MF_CAMERA_ERRORS_H_

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {
namespace errors {

inline ::felicia::Status FailedToCreateVideoCaptureDevice() {
  return Unavailable("Failed to create video capture device.");
}

inline ::felicia::Status FailedToMFCreateSourceReaderFromMediaSource(
    HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to MFCreateSourceReaderFromMediaSource: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToMFGetAttributeSize(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to MFGetAttributeSize: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToMFGetAttributeRatio(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to MFGetAttributeRatio: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToGetGUID(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to GetGUID: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

}  // namespace errors
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_MF_CAMERA_ERRORS_H_