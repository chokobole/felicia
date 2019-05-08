#ifndef FELICIA_DRIVERS_CAMERA_WIN_DSHOW_CAMERA_ERRORS_H_
#define FELICIA_DRIVERS_CAMERA_WIN_DSHOW_CAMERA_ERRORS_H_

#include "third_party/chromium/base/logging.h"
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

inline ::felicia::Status FailedToAddSinkFilter(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to add the sink filter to the graph: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToGetIAMStreamConfig(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to get IAMStreamConfig: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToGetNumberOfCapabilities(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to GetNumberOfCapabilities: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToGetStreamCaps(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to GetStreamCaps: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToSetFormat(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to SetFormat: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status IsNotAVideoType() {
  return FailedPrecondition("Current video type is not a video type");
}

inline ::felicia::Status FailedToConnectTheCaptureGraph(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to connect the CaptureGraph: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToPause(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to pause the capture device: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToRun(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to run the capture device: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

inline ::felicia::Status FailedToStop(HRESULT hr) {
  return Unavailable(
      ::base::StringPrintf("Failed to stop the capture device: %s.",
                           ::logging::SystemErrorCodeToString(hr).c_str()));
}

}  // namespace errors
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_DSHOW_CAMERA_ERRORS_H_