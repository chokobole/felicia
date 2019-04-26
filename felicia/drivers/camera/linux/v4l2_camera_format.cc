#include "felicia/drivers/camera/camera_format.h"

#include <linux/videodev2.h>

#include "third_party/chromium/base/logging.h"

namespace felicia {

namespace {

constexpr struct {
  uint32_t v4l2_pixel_format;
  CameraFormat::PixelFormat pixel_format;
} const kV4l2PixelFormatToPixelFormat[] = {
    {V4L2_PIX_FMT_YUV420, CameraFormat::PIXEL_FORMAT_I420},
    {V4L2_PIX_FMT_YVU420, CameraFormat::PIXEL_FORMAT_YV12},
    {V4L2_PIX_FMT_NV12, CameraFormat::PIXEL_FORMAT_NV12},
    {V4L2_PIX_FMT_NV21, CameraFormat::PIXEL_FORMAT_NV21},
    {V4L2_PIX_FMT_UYVY, CameraFormat::PIXEL_FORMAT_UYVY},
    {V4L2_PIX_FMT_YUYV, CameraFormat::PIXEL_FORMAT_YUY2},
    {V4L2_PIX_FMT_RGB24, CameraFormat::PIXEL_FORMAT_RGB24},
    {V4L2_PIX_FMT_RGB32, CameraFormat::PIXEL_FORMAT_RGB32},
    {V4L2_PIX_FMT_ARGB32, CameraFormat::PIXEL_FORMAT_ARGB},
    {V4L2_PIX_FMT_MJPEG, CameraFormat::PIXEL_FORMAT_MJPEG},
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
CameraFormat::PixelFormat CameraFormat::FromV4l2PixelFormat(
    uint32_t v4l2_pixel_format) {
  for (const auto& pixel_format : kV4l2PixelFormatToPixelFormat) {
    if (v4l2_pixel_format == pixel_format.v4l2_pixel_format)
      return pixel_format.pixel_format;
  }

  DLOG(WARNING) << "Unsupported format: " << FourccToString(v4l2_pixel_format);
  return PIXEL_FORMAT_UNKNOWN;
}

}  // namespace felicia