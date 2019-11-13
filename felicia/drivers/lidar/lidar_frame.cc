// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/drivers/lidar/lidar_frame.h"

namespace felicia {
namespace drivers {

LidarFrame::LidarFrame() = default;

LidarFrame::LidarFrame(float angle_start, float angle_end, float angle_delta,
                       float time_delta, float scan_time, float range_min,
                       float range_max, const Data& ranges,
                       const Data& intensities, base::TimeDelta timestamp)
    : angle_start_(angle_start),
      angle_end_(angle_end),
      angle_delta_(angle_delta),
      time_delta_(time_delta),
      scan_time_(scan_time),
      range_min_(range_min),
      range_max_(range_max),
      ranges_(ranges),
      intensities_(intensities),
      timestamp_(timestamp) {}

LidarFrame::LidarFrame(float angle_start, float angle_end, float angle_delta,
                       float time_delta, float scan_time, float range_min,
                       float range_max, Data&& ranges, Data&& intensities,
                       base::TimeDelta timestamp) noexcept
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

LidarFrame::LidarFrame(const LidarFrame& other) = default;

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

LidarFrame& LidarFrame::operator=(const LidarFrame& other) = default;

LidarFrame& LidarFrame::operator=(LidarFrame&& other) = default;

LidarFrame::~LidarFrame() = default;

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

Data& LidarFrame::ranges() { return ranges_; }

Data& LidarFrame::intensities() { return intensities_; }

const Data& LidarFrame::ranges() const { return ranges_; }

const Data& LidarFrame::intensities() const { return intensities_; }

void LidarFrame::set_timestamp(base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

base::TimeDelta LidarFrame::timestamp() const { return timestamp_; }

LidarFrameMessage LidarFrame::ToLidarFrameMessage(bool copy) {
  LidarFrameMessage message;
  message.set_angle_start(angle_start_);
  message.set_angle_end(angle_end_);
  message.set_angle_delta(angle_delta_);
  message.set_time_delta(time_delta_);
  message.set_scan_time(scan_time_);
  message.set_range_min(range_min_);
  message.set_range_max(range_max_);
  if (copy) {
    message.set_ranges(ranges_.data());
    message.set_intensities(intensities_.data());
  } else {
    message.set_ranges(std::move(ranges_).data());
    message.set_intensities(std::move(intensities_).data());
  }
  message.set_timestamp(timestamp_.InMicroseconds());
  return message;
}

Status LidarFrame::FromLidarFrameMessage(const LidarFrameMessage& message) {
  *this = LidarFrame{message.angle_start(),
                     message.angle_end(),
                     message.angle_delta(),
                     message.time_delta(),
                     message.scan_time(),
                     message.range_min(),
                     message.range_max(),
                     Data{message.ranges()},
                     Data{message.intensities()},
                     base::TimeDelta::FromMicroseconds(message.timestamp())};
  return Status::OK();
}

Status LidarFrame::FromLidarFrameMessage(LidarFrameMessage&& message) {
  std::unique_ptr<std::string> ranges(message.release_ranges());
  std::unique_ptr<std::string> intensities(message.release_intensities());

  *this = LidarFrame{message.angle_start(),
                     message.angle_end(),
                     message.angle_delta(),
                     message.time_delta(),
                     message.scan_time(),
                     message.range_min(),
                     message.range_max(),
                     Data{std::move(*ranges)},
                     Data{std::move(*intensities)},
                     base::TimeDelta::FromMicroseconds(message.timestamp())};

  return Status::OK();
}

void LidarFrame::Project(std::vector<Pointf>* points, float user_range_min,
                         float user_range_max) const {
  // TODO: Implement using BLAS.
  float range_min = std::max(user_range_min, range_min_);
  float range_max = std::min(user_range_max, range_max_);
  Data::ConstView<float> ranges = ranges_.AsConstView<float>();
  const size_t size = ranges.size();
  for (size_t i = 0; i < size; ++i) {
    float range = ranges[i];
    if (range < range_min || range > range_max) continue;
    const float radian = angle_start_ + angle_delta_ * i;
    points->emplace_back(range * std::cos(radian), range * std::sin(radian));
  }
}

}  // namespace drivers
}  // namespace felicia