#include "felicia/drivers/camera/camera_format.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/drivers/camera/camera_frame_util.h"

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

PixelFormat CameraFormat::pixel_format() const { return pixel_format_; }

void CameraFormat::set_pixel_format(PixelFormat pixel_format) {
  pixel_format_ = pixel_format;
}

int CameraFormat::width() const { return size_.width(); }

int CameraFormat::height() const { return size_.height(); }

void CameraFormat::SetSize(int width, int height) {
  size_.set_width(width);
  size_.set_height(height);
}

size_t CameraFormat::AllocationSize() const {
  return camera_internal::AllocationSize(*this);
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
const std::string& CameraFormat::PixelFormatToString(PixelFormat pixel_format) {
  return PixelFormat_Name(pixel_format);
}

CameraFormatMessage CameraFormat::ToCameraFormatMessage() const {
  CameraFormatMessage message;
  message.set_width(size_.width());
  message.set_height(size_.height());
  message.set_pixel_format(pixel_format_);
  message.set_frame_rate(frame_rate_);

  return std::move(message);
}

bool CameraFormat::operator==(const CameraFormat& other) {
  return size_ == other.size_ && pixel_format_ == other.pixel_format_ &&
         frame_rate_ == other.frame_rate_;
}

}  // namespace felicia