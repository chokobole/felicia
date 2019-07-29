#include "felicia/drivers/lidar/lidar_frame.h"

namespace felicia {

LidarFrame::LidarFrame() = default;

LidarFrame::LidarFrame(float angle_start, float angle_end, float angle_delta,
                       float time_delta, float scan_time, float range_min,
                       float range_max, std::vector<float>&& ranges,
                       std::vector<float> intensities,
                       ::base::TimeDelta timestamp) noexcept
    : angle_start_(angle_start),
      angle_end_(angle_end),
      angle_delta_(angle_delta),
      time_delta_(time_delta),
      scan_time_(scan_time),
      range_min_(range_min),
      range_max_(range_max),
      ranges_(std::move(ranges)),
      intensities_(std::move(intensities)),
      timestamp_(timestamp) {}

LidarFrame::LidarFrame(LidarFrame&& other) noexcept
    : angle_start_(other.angle_start_),
      angle_end_(other.angle_end_),
      angle_delta_(other.angle_delta_),
      time_delta_(other.time_delta_),
      scan_time_(other.scan_time_),
      range_min_(other.range_min_),
      range_max_(other.range_max_),
      ranges_(std::move(other.ranges_)),
      intensities_(std::move(other.intensities_)),
      timestamp_(other.timestamp_) {}

LidarFrame&& LidarFrame::operator=(LidarFrame&& other) {
  angle_start_ = angle_start_;
  angle_end_ = angle_end_;
  angle_delta_ = angle_delta_;
  time_delta_ = time_delta_;
  scan_time_ = scan_time_;
  range_min_ = range_min_;
  range_max_ = range_max_;
  ranges_ = std::move(other.ranges_);
  intensities_ = std::move(other.intensities_);
  timestamp_ = timestamp_;
}

void LidarFrame::set_angle_start(float angle_start) {
  angle_start_ = angle_start;
}

float LidarFrame::angle_start() const { return angle_start_; }

void LidarFrame::set_angle_end(float angle_end) { angle_end_ = angle_end; }

float LidarFrame::angle_end() const { return angle_end_; }

void LidarFrame::set_angle_delta(float angle_delta) {
  angle_delta_ = angle_delta;
}

float LidarFrame::angle_delta() const { return angle_delta_; }

void LidarFrame::set_time_delta(float time_delta) { time_delta_ = time_delta; }

float LidarFrame::time_delta() const { return time_delta_; }

void LidarFrame::set_scan_time(float scan_time) { scan_time_ = scan_time; }

float LidarFrame::scan_time() const { return scan_time_; }

void LidarFrame::set_range_min(float range_min) { range_min_ = range_min; }

float LidarFrame::range_min() const { return range_min_; }

void LidarFrame::set_range_max(float range_max) { range_max_ = range_max; }

float LidarFrame::range_max() const { return range_max_; }

std::vector<float>& LidarFrame::ranges() { return ranges_; }

const std::vector<float>& LidarFrame::ranges() const { return ranges_; }

std::vector<float>& LidarFrame::intensities() { return intensities_; }

const std::vector<float>& LidarFrame::intensities() const {
  return intensities_;
}

void LidarFrame::set_timestamp(::base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

::base::TimeDelta LidarFrame::timestamp() const { return timestamp_; }

LidarFrameMessage LidarFrame::ToLidarFrameMessage() const {
  LidarFrameMessage message;
  message.set_angle_start(angle_start_);
  message.set_angle_end(angle_end_);
  message.set_angle_delta(angle_delta_);
  message.set_time_delta(time_delta_);
  message.set_scan_time(scan_time_);
  message.set_range_min(range_min_);
  message.set_range_max(range_max_);
  for (float range : ranges_) {
    message.add_ranges(range);
  }
  for (float intensity : intensities_) {
    message.add_intensities(intensity);
  }
  message.set_timestamp(timestamp_.InMicroseconds());
  return message;
}

// static
LidarFrame LidarFrame::FromLidarFrameMessage(const LidarFrameMessage& message) {
  std::vector<float> ranges;
  ranges.reserve(message.ranges_size());
  for (float range : message.ranges()) {
    ranges.push_back(range);
  }

  std::vector<float> intensities;
  intensities.reserve(message.intensities_size());
  for (float intencity : message.intensities()) {
    intensities.push_back(intencity);
  }

  return LidarFrame{message.angle_start(),
                    message.angle_end(),
                    message.angle_delta(),
                    message.time_delta(),
                    message.scan_time(),
                    message.range_min(),
                    message.range_max(),
                    std::move(ranges),
                    std::move(intensities),
                    ::base::TimeDelta::FromMicroseconds(message.timestamp())};
}

void LidarFrame::Project(std::vector<Pointf>* points) {
  // TODO: Implement using BLAS.
  for (size_t i = 0; i < ranges_.size(); ++i) {
    float range = ranges_[i];
    if (range < range_min_ || range > range_max_) continue;
    const float radian = angle_start_ + angle_delta_ * i;
    points->emplace_back(range * std::cos(radian), range * std::sin(radian));
  }
}

}  // namespace felicia