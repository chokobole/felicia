#include "felicia/drivers/camera/camera_format.h"

#include <mfapi.h>

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/stl_util.h"

namespace felicia {
namespace drivers {

namespace {

struct {
  GUID mf_source_media_subtype;
  GUID mf_sink_media_subtype;
  PixelFormat pixel_format;
} const kMediaFormatConfigurationMap[] = {
    // IMFCaptureEngine inevitably performs the video frame decoding itself.
    // This means that the sink must always be set to an uncompressed video
    // format.

    // Since chromium uses I420 at the other end of the pipe, MF known video
    // output formats are always set to I420.
    {MFVideoFormat_I420, MFVideoFormat_I420, PIXEL_FORMAT_I420},
    {MFVideoFormat_YUY2, MFVideoFormat_I420, PIXEL_FORMAT_I420},
    {MFVideoFormat_UYVY, MFVideoFormat_I420, PIXEL_FORMAT_I420},
    {MFVideoFormat_RGB24, MFVideoFormat_I420, PIXEL_FORMAT_I420},
    {MFVideoFormat_RGB32, MFVideoFormat_I420, PIXEL_FORMAT_I420},
    {MFVideoFormat_ARGB32, MFVideoFormat_I420, PIXEL_FORMAT_I420},
    {MFVideoFormat_MJPG, MFVideoFormat_I420, PIXEL_FORMAT_I420},
    {MFVideoFormat_NV12, MFVideoFormat_I420, PIXEL_FORMAT_I420},
    {MFVideoFormat_YV12, MFVideoFormat_I420, PIXEL_FORMAT_I420},
};

}  // namespace

// static
bool CameraFormat::ToMfSinkMediaSubtype(const GUID& sub_type, GUID* sink_type) {
  for (const auto& kMediaFormatConfiguration : kMediaFormatConfigurationMap) {
    if (kMediaFormatConfiguration.mf_source_media_subtype == sub_type) {
      *sink_type = kMediaFormatConfiguration.mf_sink_media_subtype;
      return true;
    }
  }

  return false;
}

// static
PixelFormat CameraFormat::FromMfMediaSubtype(const GUID& sub_type) {
  for (const auto& kMediaFormatConfiguration : kMediaFormatConfigurationMap) {
    if (kMediaFormatConfiguration.mf_source_media_subtype == sub_type) {
      return kMediaFormatConfiguration.pixel_format;
    }
  }

#ifndef NDEBUG
  WCHAR guid_str[128];
  StringFromGUID2(sub_type, guid_str, base::size(guid_str));
  DLOG(WARNING) << "Unsupported format: " << guid_str;
#endif

  return PIXEL_FORMAT_UNKNOWN;
}

}  // namespace drivers
}  // namespace felicia