#include "felicia/drivers/camera/realsense/rs_pixel_format.h"

#include "third_party/chromium/base/logging.h"

namespace felicia {

namespace {

constexpr struct {
  rs2_format rs2_pixel_format;
  PixelFormat pixel_format;
} const kRs2FormatToPixelFormat[] = {
    {RS2_FORMAT_Z16, PIXEL_FORMAT_Z16},
    {RS2_FORMAT_YUYV, PIXEL_FORMAT_YUY2},
    {RS2_FORMAT_RGB8, PIXEL_FORMAT_XBGR},
    {RS2_FORMAT_BGR8, PIXEL_FORMAT_RGB24},
    {RS2_FORMAT_RGBA8, PIXEL_FORMAT_RGB32},
    {RS2_FORMAT_BGRA8, PIXEL_FORMAT_ARGB},
    {RS2_FORMAT_Y8, PIXEL_FORMAT_Y8},
    {RS2_FORMAT_Y16, PIXEL_FORMAT_Y16},
    {RS2_FORMAT_UYVY, PIXEL_FORMAT_UYVY},
};

}  // namespace

PixelFormat FromRs2Format(rs2_format rs2_format) {
  for (const auto& rs2_format_to_pixel_format : kRs2FormatToPixelFormat) {
    if (rs2_format == rs2_format_to_pixel_format.rs2_pixel_format)
      return rs2_format_to_pixel_format.pixel_format;
  }
  DLOG(WARNING) << "Unsupported format: " << rs2_format_to_string(rs2_format);
  return PIXEL_FORMAT_UNKNOWN;
}

rs2_format ToRs2Format(PixelFormat pixel_format) {
  for (const auto& rs2_format_to_pixel_format : kRs2FormatToPixelFormat) {
    if (pixel_format == rs2_format_to_pixel_format.pixel_format)
      return rs2_format_to_pixel_format.rs2_pixel_format;
  }
  DLOG(WARNING) << "Unsupported format: " << PixelFormat_Name(pixel_format);
  return RS2_FORMAT_ANY;
}

}  // namespace felicia
