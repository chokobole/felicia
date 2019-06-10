#include "felicia/drivers/lidar/lidar_frame.h"

namespace felicia {

LidarFrame::LidarFrame() = default;

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

}  // namespace felicia