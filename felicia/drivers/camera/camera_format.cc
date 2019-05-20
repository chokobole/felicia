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

CameraFormat::CameraFormat(const CameraFormat& camera_format) = default;

CameraFormat& CameraFormat::operator=(const CameraFormat& camera_format) =
    default;

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

  return message;
}

bool CameraFormat::operator==(const CameraFormat& other) {
  return size_ == other.size_ && pixel_format_ == other.pixel_format_ &&
         frame_rate_ == other.frame_rate_;
}

std::ostream& operator<<(std::ostream& os, const CameraFormat& camera_format) {
  os << camera_format.ToString();
  return os;
}

// This list is ordered by precedence of use.
static PixelFormat const kSupportedCapturePixelFormats[] = {
    PIXEL_FORMAT_I420,  PIXEL_FORMAT_YV12,  PIXEL_FORMAT_NV12,
    PIXEL_FORMAT_NV21,  PIXEL_FORMAT_UYVY,  PIXEL_FORMAT_YUY2,
    PIXEL_FORMAT_RGB24, PIXEL_FORMAT_RGB32, PIXEL_FORMAT_ARGB,
    PIXEL_FORMAT_MJPEG,
};

bool ComparePixelFormatPreference(PixelFormat lhs, PixelFormat rhs) {
  auto* format_lhs = std::find(
      kSupportedCapturePixelFormats,
      kSupportedCapturePixelFormats + base::size(kSupportedCapturePixelFormats),
      lhs);
  auto* format_rhs = std::find(
      kSupportedCapturePixelFormats,
      kSupportedCapturePixelFormats + base::size(kSupportedCapturePixelFormats),
      rhs);
  return format_lhs < format_rhs;
}

bool CompareCapability(const CameraFormat& requested, const CameraFormat& lhs,
                       const CameraFormat& rhs) {
  if (lhs.pixel_format() == requested.pixel_format() &&
      rhs.pixel_format() != requested.pixel_format()) {
    return true;
  } else if (lhs.pixel_format() != requested.pixel_format() &&
             lhs.pixel_format() == requested.pixel_format()) {
    return false;
  }

  const int diff_height_lhs = std::abs(lhs.height() - requested.height());
  const int diff_height_rhs = std::abs(rhs.height() - requested.height());
  if (diff_height_lhs != diff_height_rhs)
    return diff_height_lhs < diff_height_rhs;

  const int diff_width_lhs = std::abs(lhs.width() - requested.width());
  const int diff_width_rhs = std::abs(rhs.width() - requested.width());
  if (diff_width_lhs != diff_width_rhs) return diff_width_lhs < diff_width_rhs;

  const float diff_fps_lhs =
      std::fabs(lhs.frame_rate() - requested.frame_rate());
  const float diff_fps_rhs =
      std::fabs(rhs.frame_rate() - requested.frame_rate());
  if (diff_fps_lhs != diff_fps_rhs) return diff_fps_lhs < diff_fps_rhs;

  return ComparePixelFormatPreference(lhs.pixel_format(), rhs.pixel_format());
}

const CameraFormat& GetBestMatchedCameraFormat(
    const CameraFormat& requested, const CameraFormats& camera_formats) {
  DCHECK(!camera_formats.empty());
  const CameraFormat* best_match = &(*camera_formats.begin());
  for (const CameraFormat& camera_format : camera_formats) {
    if (CompareCapability(requested, camera_format, *best_match)) {
      best_match = &camera_format;
    }
  }
  return *best_match;
}

}  // namespace felicia