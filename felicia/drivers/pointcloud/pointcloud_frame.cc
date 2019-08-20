#include "felicia/drivers/pointcloud/pointcloud_frame.h"

namespace felicia {
namespace drivers {

PointcloudFrame::PointcloudFrame() = default;

PointcloudFrame::PointcloudFrame(size_t points_size, size_t colors_size) {
  points_.reserve(points_size);
  colors_.reserve(colors_size);
}

PointcloudFrame::PointcloudFrame(const std::string& points,
                                 const std::string& colors,
                                 base::TimeDelta timestamp)
    : points_(points), colors_(colors), timestamp_(timestamp) {}

PointcloudFrame::PointcloudFrame(std::string&& points, std::string&& colors,
                                 base::TimeDelta timestamp) noexcept
    : points_(std::move(points)),
      colors_(std::move(colors)),
      timestamp_(timestamp) {}

PointcloudFrame::PointcloudFrame(const PointcloudFrame& other)
    : points_(other.points_),
      colors_(other.colors_),
      timestamp_(other.timestamp_) {}

PointcloudFrame::PointcloudFrame(PointcloudFrame&& other) noexcept
    : points_(std::move(other.points_)),
      colors_(std::move(other.colors_)),
      timestamp_(other.timestamp_) {}

PointcloudFrame& PointcloudFrame::operator=(const PointcloudFrame& other) =
    default;
PointcloudFrame& PointcloudFrame::operator=(PointcloudFrame&& other) = default;

void PointcloudFrame::AddPoint(float x, float y, float z) {
  AddPoint(Point3f{x, y, z});
}

void PointcloudFrame::AddPoint(const Point3f& point) {
  points_.push_back<Point3f>(point);
}

void PointcloudFrame::AddColor(uint8_t r, uint8_t g, uint8_t b) {
  AddColor(Color3u{r, g, b});
}

void PointcloudFrame::AddColor(const Color3u& color) {
  colors_.push_back<Color3u>(color);
}

void PointcloudFrame::AddPointAndColor(float x, float y, float z, uint8_t r,
                                       uint8_t g, uint8_t b) {
  AddPoint(x, y, z);
  AddColor(r, g, b);
}

Point3f& PointcloudFrame::PointAt(size_t idx) {
  return points_.at<Point3f>(idx);
}

Color3u& PointcloudFrame::ColorAt(size_t idx) {
  return colors_.at<Color3u>(idx);
}

const Point3f& PointcloudFrame::PointAt(size_t idx) const {
  return points_.at<Point3f>(idx);
}

const Color3u& PointcloudFrame::ColorAt(size_t idx) const {
  return colors_.at<Color3u>(idx);
}

void PointcloudFrame::set_timestamp(base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

base::TimeDelta PointcloudFrame::timestamp() const { return timestamp_; }

PointcloudFrameMessage PointcloudFrame::ToPointcloudFrameMessage() const {
  PointcloudFrameMessage message;
  message.set_points(points_.data());
  message.set_colors(colors_.data());
  message.set_timestamp(timestamp_.InMicroseconds());
  return message;
}

Status PointcloudFrame::FromPointcloudFrameMessage(
    const PointcloudFrameMessage& message) {
  const std::string& points = message.points();
  const std::string& colors = message.colors();

  *this = PointcloudFrame{
      points, colors, base::TimeDelta::FromMicroseconds(message.timestamp())};

  return Status::OK();
}

Status PointcloudFrame::FromPointcloudFrameMessage(
    PointcloudFrameMessage&& message) {
  std::unique_ptr<std::string> points(message.release_points());
  std::unique_ptr<std::string> colors(message.release_colors());

  *this =
      PointcloudFrame{std::move(*points), std::move(*colors),
                      base::TimeDelta::FromMicroseconds(message.timestamp())};

  return Status::OK();
}

}  // namespace drivers
}  // namespace felicia