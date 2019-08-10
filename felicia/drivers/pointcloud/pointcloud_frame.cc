#include "felicia/drivers/pointcloud/pointcloud_frame.h"

namespace felicia {
namespace drivers {

PointcloudFrame::PointcloudFrame() = default;

PointcloudFrame::PointcloudFrame(size_t points_size, size_t colors_size) {
  points_.reserve(points_size);
  colors_.reserve(colors_size);
}

PointcloudFrame::PointcloudFrame(std::vector<Point3f>&& points,
                                 std::vector<uint8_t>&& colors,
                                 base::TimeDelta timestamp) noexcept
    : points_(std::move(points)),
      colors_(std::move(colors)),
      timestamp_(timestamp) {}

PointcloudFrame::PointcloudFrame(PointcloudFrame&& other) noexcept
    : points_(std::move(other.points_)),
      colors_(std::move(other.colors_)),
      timestamp_(other.timestamp_) {}

PointcloudFrame& PointcloudFrame::operator=(PointcloudFrame&& other) = default;

void PointcloudFrame::AddPoint(float x, float y, float z) {
  points_.emplace_back(x, y, z);
}

void PointcloudFrame::AddPoint(const Point3f& point) {
  points_.push_back(point);
}

void PointcloudFrame::AddPointAndColor(float x, float y, float z, uint8_t r,
                                       uint8_t g, uint8_t b) {
  points_.emplace_back(x, y, z);
  colors_.push_back(r);
  colors_.push_back(g);
  colors_.push_back(b);
}

void PointcloudFrame::set_timestamp(base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

base::TimeDelta PointcloudFrame::timestamp() const { return timestamp_; }

PointcloudFrameMessage PointcloudFrame::ToPointcloudFrameMessage() const {
  PointcloudFrameMessage message;
  message.set_points(points_.data(), points_.size() * sizeof(Point3f));
  message.set_colors(colors_.data(), colors_.size() * sizeof(uint8_t));
  message.set_timestamp(timestamp_.InMicroseconds());
  return message;
}

// static
PointcloudFrame PointcloudFrame::FromPointcloudFrameMessage(
    const PointcloudFrameMessage& message) {
  std::vector<Point3f> points;
  const std::string& points_data = message.points();
  points.resize(points_data.length() / sizeof(Point3f));
  memcpy(points.data(), points_data.c_str(), points_data.length());

  std::vector<uint8_t> colors;
  const std::string& colors_data = message.colors();
  colors.resize(colors_data.length() / sizeof(uint8_t));
  memcpy(colors.data(), colors_data.c_str(), colors_data.length());

  return {std::move(points), std::move(colors),
          base::TimeDelta::FromMicroseconds(message.timestamp())};
}

}  // namespace drivers
}  // namespace felicia