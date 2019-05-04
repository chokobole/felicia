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

inline ::felicia::Status MediaTypeNotMatched() {
  return InvalidArgument("MediaType not matched.");
}

inline ::felicia::Status FailedToCreateCaptureEngine(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to create CaptureEngine: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToInitialize(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to Initialize: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToGetSource(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to GetSource: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToGetSink(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to GetSink: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToQueryCapturePreviewSinkInterface(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to query CapturePreviewSinkInterface: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToRemoveAllStreams(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to RemoveAllStreams: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToCreateSinkVideoMediaType(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to create SinkVideoMediaType: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToConvertToVideoSinkMediaType(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to ConvertToVideoSinkMediaType: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToAddStream(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to AddStream: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToSetSampleCallback(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to SetSampleCallback: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToStartPreview(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to StartPreview: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToStopPreview(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to StopPreview: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToFillVideoCapabilities(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to FillVideoCapabilities: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status MediaEventStatusFailed(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("MediaEventStatusFailed: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToGetAvailableDeviceMediaType(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to GetAvailableDeviceMediaType: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToSetCurrentDeviceMediaType(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to SetCurrentDeviceMediaType: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

}  // namespace errors
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_MF_CAMERA_ERRORS_H_