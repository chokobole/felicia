#include "felicia/drivers/pointcloud/pointcloud_frame.h"

#include "felicia/core/lib/unit/geometry/geometry_util.h"

namespace felicia {

PointcloudFrame::PointcloudFrame()
    : points_(std::make_unique<std::vector<Point3f>>()),
      colors_(std::make_unique<std::vector<Color>>()) {}

PointcloudFrame::PointcloudFrame(size_t points_size, size_t colors_size)
    : points_(std::make_unique<std::vector<Point3f>>()),
      colors_(std::make_unique<std::vector<Color>>()) {
  points_->reserve(points_size);
  colors_->reserve(colors_size);
}

PointcloudFrame::PointcloudFrame(PointcloudFrame&& other) noexcept
    : points_(std::move(other.points_)),
      colors_(std::move(other.colors_)),
      timestamp_(other.timestamp_) {}

void PointcloudFrame::operator=(PointcloudFrame&& other) {
  points_ = std::move(points_);
  colors_ = std::move(colors_);
  timestamp_ = other.timestamp_;
}

void PointcloudFrame::AddPoint(float x, float y, float z) {
  points_->emplace_back(x, y, z);
}

void PointcloudFrame::AddPoint(const Point3f& point) {
  points_->push_back(point);
}

void PointcloudFrame::AddPointAndColor(float x, float y, float z, float r,
                                       float g, float b) {
  points_->emplace_back(x, y, z);
  colors_->emplace_back(r, g, b);
}

void PointcloudFrame::AddPointAndColor(const Point3f& point, const Color& color) {
  points_->push_back(point);
  colors_->push_back(color);
}

void PointcloudFrame::set_timestamp(::base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

::base::TimeDelta PointcloudFrame::timestamp() const { return timestamp_; }

PointcloudFrameMessage PointcloudFrame::ToPointcloudFrameMessage() const {
  PointcloudFrameMessage message;
  message.set_points(points_->data(), points_->size() * sizeof(Point3f));
  message.set_colors(colors_->data(), colors_->size() * sizeof(Color));
  message.set_timestamp(timestamp_.InMicroseconds());
  return message;
}

}  // namespace felicia