#ifndef FELICIA_DRIVERS_CAMERA_WIN_DSHOW_CAMERA_ERRORS_H_
#define FELICIA_DRIVERS_CAMERA_WIN_DSHOW_CAMERA_ERRORS_H_

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {
namespace errors {

inline ::felicia::Status FailedToCreateCaptureFilter(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to create capture filter: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FaieldToGetCaptureOutputPin() {
  return Unavailable("Failed to get capture output pin.");
}

inline ::felicia::Status FailedToCreateSinkFilter() {
  return Unavailable("Failed to create sink filter.");
}

inline ::felicia::Status FailedToCreateCaptureGraphBuilder(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to create the Capture Graph Builder: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToSetGraphBuilderFilterGraph(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to give graph to capture graph builder: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToCreateMediaControlBuilder(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to create media control builder: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToAddCaptureFilter(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to add the capture device to the graph: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedtoAddSinkFilter(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to add the sink filter to the graph: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToGetIAMStreamConfig(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to get IAMStreamConfig: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToGetFormatFromIAMStreamConfig(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to get format from IAMStreamConfig: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status IsNotAVideoType() {
  return FailedPrecondition("Current video type is not a video type");
}

inline ::felicia::Status FailedToGetIAMVideoControl(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to get IAMVideoControl: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToConnectTheCaptureGraph(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to connect the Capture graph: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToPause(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to pause the Capture device: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToRun(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to run the Capture device: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

}  // namespace errors
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_DSHOW_CAMERA_ERRORS_H_