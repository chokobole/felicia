#include "felicia/drivers/pointcloud/pointcloud_frame.h"

namespace felicia {
namespace drivers {

PointcloudFrame::PointcloudFrame() = default;

PointcloudFrame::PointcloudFrame(const Data& points, const Data& colors,
                                 base::TimeDelta timestamp)
    : points_(points), colors_(colors), timestamp_(timestamp) {}

PointcloudFrame::PointcloudFrame(Data&& points, Data&& colors,
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

const Data& PointcloudFrame::points() const { return points_; }

Data& PointcloudFrame::points() { return points_; }

const Data& PointcloudFrame::colors() const { return colors_; }

Data& PointcloudFrame::colors() { return colors_; }

void PointcloudFrame::set_timestamp(base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

base::TimeDelta PointcloudFrame::timestamp() const { return timestamp_; }

PointcloudFrameMessage PointcloudFrame::ToPointcloudFrameMessage(bool copy) {
  PointcloudFrameMessage message;
  *message.mutable_points() = points_.ToDataMessage(copy);
  *message.mutable_colors() = colors_.ToDataMessage(copy);
  message.set_timestamp(timestamp_.InMicroseconds());
  return message;
}

Status PointcloudFrame::FromPointcloudFrameMessage(
    const PointcloudFrameMessage& message) {
  Data points;
  Status s = points.FromDataMessage(message.points());
  if (!s.ok()) return s;
  Data colors;
  s = colors.FromDataMessage(message.colors());
  if (!s.ok()) return s;

  *this =
      PointcloudFrame{std::move(points), std::move(colors),
                      base::TimeDelta::FromMicroseconds(message.timestamp())};

  return Status::OK();
}

Status PointcloudFrame::FromPointcloudFrameMessage(
    PointcloudFrameMessage&& message) {
  std::unique_ptr<DataMessage> points_message(message.release_points());
  Data points;
  Status s = points.FromDataMessage(std::move(*points_message));
  if (!s.ok()) return s;
  std::unique_ptr<DataMessage> colors_message(message.release_colors());
  Data colors;
  s = colors.FromDataMessage(std::move(*colors_message));
  if (!s.ok()) return s;

  *this =
      PointcloudFrame{std::move(points), std::move(colors),
                      base::TimeDelta::FromMicroseconds(message.timestamp())};

  return Status::OK();
}

}  // namespace drivers
}  // namespace felicia