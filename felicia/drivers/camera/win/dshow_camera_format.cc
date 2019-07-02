#include "felicia/drivers/camera/camera_format.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/stl_util.h"

#include "felicia/drivers/camera/win/sink_filter.h"

namespace felicia {

namespace {

struct {
  const GUID& sub_type;
  PixelFormat pixel_format;
} const kMediaSubtypeToPixelFormatCorrespondence[] = {
    {kMediaSubTypeI420, PIXEL_FORMAT_I420},
    {MEDIASUBTYPE_IYUV, PIXEL_FORMAT_I420},
    {MEDIASUBTYPE_RGB24, PIXEL_FORMAT_BGR},
    {MEDIASUBTYPE_RGB32, PIXEL_FORMAT_BGRX},
    {MEDIASUBTYPE_YUY2, PIXEL_FORMAT_YUY2},
    {MEDIASUBTYPE_MJPG, PIXEL_FORMAT_MJPEG},
    {MEDIASUBTYPE_UYVY, PIXEL_FORMAT_UYVY},
    {MEDIASUBTYPE_ARGB32, PIXEL_FORMAT_BGRA},
    {kMediaSubTypeHDYC, PIXEL_FORMAT_UYVY},
    {kMediaSubTypeY16, PIXEL_FORMAT_Y16},
    {kMediaSubTypeZ16, PIXEL_FORMAT_Z16},
    {kMediaSubTypeINVZ, PIXEL_FORMAT_Z16},
};

}  // namespace

const GUID& CameraFormat::ToDshowMediaSubtype() const {
  for (const auto& pixel_format : kMediaSubtypeToPixelFormatCorrespondence) {
    if (pixel_format_ == pixel_format.pixel_format)
      return pixel_format.sub_type;
  }

  DLOG(WARNING) << "Unsupported format: " << PixelFormatToString(pixel_format_);
  return MEDIASUBTYPE_RGB24;
}

// static
PixelFormat CameraFormat::FromDshowMediaSubtype(const GUID& sub_type) {
  for (const auto& pixel_format : kMediaSubtypeToPixelFormatCorrespondence) {
    if (sub_type == pixel_format.sub_type) return pixel_format.pixel_format;
  }

#ifndef NDEBUG
  WCHAR guid_str[128];
  StringFromGUID2(sub_type, guid_str, ::base::size(guid_str));
  DLOG(WARNING) << "Unsupported format: " << guid_str;
#endif

  return PIXEL_FORMAT_UNKNOWN;
}

}  // namespace felicia