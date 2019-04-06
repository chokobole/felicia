#include "felicia/drivers/camera/camera_format.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

namespace felicia {

CameraFormat::CameraFormat() = default;

CameraFormat::CameraFormat(int width, int height, PixelFormat pixel_format)
    : pixel_format_(pixel_format), size_(Sizei(width, height)) {}

CameraFormat::PixelFormat CameraFormat::pixel_format() const {
  return pixel_format_;
}

int CameraFormat::width() const { return size_.width(); }

int CameraFormat::height() const { return size_.height(); }

// static
std::string CameraFormat::FourccToString(uint32_t fourcc) {
  std::string result = "0000";
  for (size_t i = 0; i < 4; ++i, fourcc >>= 8) {
    const char c = static_cast<char>(fourcc & 0xFF);
    if (c <= 0x1f || c >= 0x7f) return base::StringPrintf("0x%x", fourcc);
    result[i] = c;
  }
  return result;
}

std::string CameraFormat::ToString() const {
  return ::base::StringPrintf("%dx%d(%s)", width(), height(),
                              PixelFormatToString(pixel_format_).c_str());
}

// static
std::string CameraFormat::PixelFormatToString(PixelFormat pixel_format) {
  switch (pixel_format) {
#define PIXEL_FORMAT(format, v4l2_format) \
  case format:                            \
    return #format;
#include "felicia/drivers/camera/camera_format_list.h"
#undef PIXEL_FORMAT
    default:
      NOTREACHED();
      return ::base::EmptyString();
  }
}

}  // namespace felicia