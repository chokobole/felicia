#include "felicia/drivers/imu/imu_format.h"

#include "third_party/chromium/base/strings/stringprintf.h"

namespace felicia {

ImuFormat::ImuFormat() = default;

ImuFormat::ImuFormat(float frame_rate) : frame_rate_(frame_rate) {}

ImuFormat::ImuFormat(const ImuFormat& other) = default;

ImuFormat& ImuFormat::operator=(const ImuFormat& other) = default;

std::string ImuFormat::ToString() const {
  return ::base::StringPrintf("%.2fHz", frame_rate_);
}

float ImuFormat::frame_rate() const { return frame_rate_; }

void ImuFormat::set_frame_rate(float frame_rate) { frame_rate_ = frame_rate; }

ImuFormatMessage ImuFormat::ToImuFormatMessage() const {
  ImuFormatMessage message;
  message.set_frame_rate(frame_rate_);
  return message;
}

bool ImuFormat::operator==(const ImuFormat& other) {
  return frame_rate_ == other.frame_rate_;
}

std::ostream& operator<<(std::ostream& os, const ImuFormat& imu_format) {
  os << imu_format.ToString();
  return os;
}

bool CompareCapability(const ImuFormat& requested, const ImuFormat& lhs,
                       const ImuFormat& rhs) {
  const float diff_fps_lhs =
      std::fabs(lhs.frame_rate() - requested.frame_rate());
  const float diff_fps_rhs =
      std::fabs(rhs.frame_rate() - requested.frame_rate());
  return diff_fps_lhs < diff_fps_rhs;
}

}  // namespace felicia