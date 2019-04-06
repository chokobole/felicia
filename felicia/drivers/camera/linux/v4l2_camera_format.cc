#include "felicia/drivers/camera/camera_format.h"

#include <linux/videodev2.h>

#include "third_party/chromium/base/logging.h"

namespace felicia {

uint32_t CameraFormat::ToV4l2PixelFormat() const {
  switch (pixel_format_) {
#define PIXEL_FORMAT(format, v4l2_format) \
  case format:                            \
    return v4l2_format;
#include "felicia/drivers/camera/camera_format_list.h"
#undef PIXEL_FORMAT
  }
  NOTREACHED();
  return V4L2_PIX_FMT_RGB24;
}

// static
CameraFormat::PixelFormat CameraFormat::FromV4l2PixelFormat(
    uint32_t pixel_format) {
  LOG(INFO) << FourccToString(pixel_format);
  switch (pixel_format) {
#define PIXEL_FORMAT(format, v4l2_format) \
  case v4l2_format:                       \
    return format;
#include "felicia/drivers/camera/camera_format_list.h"
#undef PIXEL_FORMAT
  }
  NOTREACHED();
  return PIXEL_FORMAT_UNKNOWN;
}

}  // namespace felicia