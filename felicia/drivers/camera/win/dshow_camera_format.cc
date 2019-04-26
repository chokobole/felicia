#include "felicia/drivers/camera/camera_format.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/stl_util.h"

#include "felicia/drivers/camera/win/sink_filter.h"

namespace felicia {

namespace {

static struct {
  const GUID& sub_type;
  CameraFormat::PixelFormat pixel_format;
} const kMediaSubtypeToPixelFormatCorrespondence[] = {
    {kMediaSubTypeI420, CameraFormat::PIXEL_FORMAT_I420},
    {MEDIASUBTYPE_IYUV, CameraFormat::PIXEL_FORMAT_I420},
    {MEDIASUBTYPE_RGB24, CameraFormat::PIXEL_FORMAT_RGB24},
    {MEDIASUBTYPE_RGB32, CameraFormat::PIXEL_FORMAT_RGB32},
    {MEDIASUBTYPE_YUY2, CameraFormat::PIXEL_FORMAT_YUY2},
    {MEDIASUBTYPE_MJPG, CameraFormat::PIXEL_FORMAT_MJPEG},
    {MEDIASUBTYPE_UYVY, CameraFormat::PIXEL_FORMAT_UYVY},
    {MEDIASUBTYPE_ARGB32, CameraFormat::PIXEL_FORMAT_ARGB},
    {kMediaSubTypeHDYC, CameraFormat::PIXEL_FORMAT_UYVY},
    {kMediaSubTypeY16, CameraFormat::PIXEL_FORMAT_Y16},
    {kMediaSubTypeZ16, CameraFormat::PIXEL_FORMAT_Y16},
    {kMediaSubTypeINVZ, CameraFormat::PIXEL_FORMAT_Y16},
};

}  // namespace

const GUID& CameraFormat::ToMediaSubtype() const {
  for (const auto& pixel_format : kMediaSubtypeToPixelFormatCorrespondence) {
    if (pixel_format_ == pixel_format.pixel_format)
      return pixel_format.sub_type;
  }

  DLOG(WARNING) << "Unsupported format: " << PixelFormatToString(pixel_format_);
  return MEDIASUBTYPE_RGB24;
}

// static
CameraFormat::PixelFormat CameraFormat::FromMediaSubtype(const GUID& sub_type) {
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