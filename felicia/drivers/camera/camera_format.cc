#include "felicia/drivers/camera/camera_format.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

namespace felicia {

CameraFormat::CameraFormat() = default;

CameraFormat::CameraFormat(Sizei size, PixelFormat pixel_format,
                           float frame_rate)
    : size_(size), pixel_format_(pixel_format), frame_rate_(frame_rate) {}

CameraFormat::CameraFormat(int width, int height, PixelFormat pixel_format,
                           float frame_rate)
    : size_(Sizei(width, height)),
      pixel_format_(pixel_format),
      frame_rate_(frame_rate) {}

CameraFormat::PixelFormat CameraFormat::pixel_format() const {
  return pixel_format_;
}

void CameraFormat::set_pixel_format(PixelFormat pixel_format) {
  pixel_format_ = pixel_format;
}

int CameraFormat::width() const { return size_.width(); }

int CameraFormat::height() const { return size_.height(); }

void CameraFormat::SetSize(int width, int height) {
  size_.set_width(width);
  size_.set_height(height);
}

float CameraFormat::frame_rate() const { return frame_rate_; }

void CameraFormat::set_frame_rate(float frame_rate) {
  frame_rate_ = frame_rate;
}

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
  return ::base::StringPrintf("%dx%d(%s) %.2fHz", width(), height(),
                              PixelFormatToString(pixel_format_).c_str(),
                              frame_rate_);
}

// static
std::string CameraFormat::PixelFormatToString(PixelFormat pixel_format) {
  switch (pixel_format) {
#define PIXEL_FORMAT(format) \
  case format:               \
    return #format;
#include "felicia/drivers/camera/camera_format_list.h"
#undef PIXEL_FORMAT
    default:
      NOTREACHED();
      return ::base::EmptyString();
  }
}

}  // namespace felicia