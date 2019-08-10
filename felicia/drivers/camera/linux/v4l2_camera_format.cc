#include "felicia/drivers/camera/camera_format.h"

#include <linux/videodev2.h>

#include "third_party/chromium/base/logging.h"

namespace felicia {
namespace drivers {

namespace {

constexpr struct {
  uint32_t v4l2_pixel_format;
  PixelFormat pixel_format;
} const kV4l2PixelFormatToPixelFormat[] = {
    {V4L2_PIX_FMT_YUV420, PIXEL_FORMAT_I420},
    {V4L2_PIX_FMT_YVU420, PIXEL_FORMAT_YV12},
    {V4L2_PIX_FMT_NV12, PIXEL_FORMAT_NV12},
    {V4L2_PIX_FMT_NV21, PIXEL_FORMAT_NV21},
    {V4L2_PIX_FMT_UYVY, PIXEL_FORMAT_UYVY},
    {V4L2_PIX_FMT_YUYV, PIXEL_FORMAT_YUY2},
    {V4L2_PIX_FMT_ABGR32, PIXEL_FORMAT_BGRA},
    {V4L2_PIX_FMT_BGR24, PIXEL_FORMAT_BGR},
    {V4L2_PIX_FMT_ARGB32, PIXEL_FORMAT_ARGB},
    {V4L2_PIX_FMT_MJPEG, PIXEL_FORMAT_MJPEG},
    {V4L2_PIX_FMT_RGB32, PIXEL_FORMAT_RGB},

};

}  // namespace

uint32_t CameraFormat::ToV4l2PixelFormat() const {
  for (const auto& pixel_format : kV4l2PixelFormatToPixelFormat) {
    if (pixel_format_ == pixel_format.pixel_format)
      return pixel_format.v4l2_pixel_format;
  }

  DLOG(WARNING) << "Unsupported format: " << PixelFormatToString(pixel_format_);
  return V4L2_PIX_FMT_RGB24;
}

// static
PixelFormat CameraFormat::FromV4l2PixelFormat(uint32_t v4l2_pixel_format) {
  for (const auto& pixel_format : kV4l2PixelFormatToPixelFormat) {
    if (v4l2_pixel_format == pixel_format.v4l2_pixel_format)
      return pixel_format.pixel_format;
  }

  DLOG(WARNING) << "Unsupported format: " << FourccToString(v4l2_pixel_format);
  return PIXEL_FORMAT_UNKNOWN;
}

}  // namespace drivers
}  // namespace felicia