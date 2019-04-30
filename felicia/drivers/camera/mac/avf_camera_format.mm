#include "felicia/drivers/camera/camera_format.h"

#import <CoreMedia/CoreMedia.h>
#import <Foundation/Foundation.h>

#include "third_party/chromium/base/logging.h"

namespace felicia {

namespace {

constexpr struct {
  FourCharCode avf_pixel_format;
  CameraFormat::PixelFormat pixel_format;
} const kAVFoundationPixelFormatToPixelFormat[] = {
    {kCVPixelFormatType_422YpCbCr8, CameraFormat::PIXEL_FORMAT_UYVY},
    {kCMPixelFormat_422YpCbCr8_yuvs, CameraFormat::PIXEL_FORMAT_YUY2},
    {kCMVideoCodecType_JPEG_OpenDML, CameraFormat::PIXEL_FORMAT_MJPEG},
};

}  // namespace

FourCharCode CameraFormat::ToAVFoundationPixelFormat() const {
  for (const auto& pixel_format : kAVFoundationPixelFormatToPixelFormat) {
    if (pixel_format_ == pixel_format.pixel_format) return pixel_format.avf_pixel_format;
  }

  DLOG(WARNING) << "Unsupported format: " << PixelFormatToString(pixel_format_);
  return kCVPixelFormatType_422YpCbCr8;
}

// static
CameraFormat::PixelFormat CameraFormat::FromAVFoundationPixelFormat(
    const FourCharCode avf_pixel_format) {
  for (const auto& pixel_format : kAVFoundationPixelFormatToPixelFormat) {
    if (avf_pixel_format == pixel_format.avf_pixel_format) return pixel_format.pixel_format;
  }

  DLOG(WARNING) << "Unsupported format: " << FourccToString(avf_pixel_format);
  return PIXEL_FORMAT_UNKNOWN;
}

}  // namespace felicia