#include "felicia/drivers/pointcloud/pointcloud_frame.h"

namespace felicia {
namespace drivers {

PointcloudFrame::PointcloudFrame() = default;

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

const StringVector& PointcloudFrame::points() const { return points_; }

StringVector& PointcloudFrame::points() { return points_; }

const StringVector& PointcloudFrame::colors() const { return colors_; }

StringVector& PointcloudFrame::colors() { return colors_; }

void PointcloudFrame::set_timestamp(base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

base::TimeDelta PointcloudFrame::timestamp() const { return timestamp_; }

PointcloudFrameMessage PointcloudFrame::ToPointcloudFrameMessage(bool copy) {
  PointcloudFrameMessage message;
  if (copy) {
    message.set_points(points_.data());
    message.set_colors(colors_.data());
  } else {
    message.set_points(std::move(points_).data());
    message.set_colors(std::move(colors_).data());
  }
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