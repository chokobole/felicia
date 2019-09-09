#include "felicia/map/pointcloud.h"

namespace felicia {
namespace map {

Pointcloud::Pointcloud() = default;

Pointcloud::Pointcloud(const Data& points, const Data& colors,
                       base::TimeDelta timestamp)
    : points_(points), colors_(colors), timestamp_(timestamp) {}

Pointcloud::Pointcloud(Data&& points, Data&& colors,
                       base::TimeDelta timestamp) noexcept
    : points_(std::move(points)),
      colors_(std::move(colors)),
      timestamp_(timestamp) {}

Pointcloud::Pointcloud(const Pointcloud& other)
    : points_(other.points_),
      colors_(other.colors_),
      timestamp_(other.timestamp_) {}

Pointcloud::Pointcloud(Pointcloud&& other) noexcept
    : points_(std::move(other.points_)),
      colors_(std::move(other.colors_)),
      timestamp_(other.timestamp_) {}

Pointcloud& Pointcloud::operator=(const Pointcloud& other) = default;
Pointcloud& Pointcloud::operator=(Pointcloud&& other) = default;

const Data& Pointcloud::points() const { return points_; }

Data& Pointcloud::points() { return points_; }

const Data& Pointcloud::colors() const { return colors_; }

Data& Pointcloud::colors() { return colors_; }

void Pointcloud::set_timestamp(base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

base::TimeDelta Pointcloud::timestamp() const { return timestamp_; }

PointcloudMessage Pointcloud::ToPointcloudMessage(bool copy) {
  PointcloudMessage message;
  *message.mutable_points() = points_.ToDataMessage(copy);
  *message.mutable_colors() = colors_.ToDataMessage(copy);
  message.set_timestamp(timestamp_.InMicroseconds());
  return message;
}

Status Pointcloud::FromPointcloudMessage(const PointcloudMessage& message) {
  Data points;
  Status s = points.FromDataMessage(message.points());
  if (!s.ok()) return s;
  Data colors;
  s = colors.FromDataMessage(message.colors());
  if (!s.ok()) return s;

  *this = Pointcloud{std::move(points), std::move(colors),
                     base::TimeDelta::FromMicroseconds(message.timestamp())};

  return Status::OK();
}

Status Pointcloud::FromPointcloudMessage(PointcloudMessage&& message) {
  std::unique_ptr<DataMessage> points_message(message.release_points());
  Data points;
  Status s = points.FromDataMessage(std::move(*points_message));
  if (!s.ok()) return s;
  std::unique_ptr<DataMessage> colors_message(message.release_colors());
  Data colors;
  s = colors.FromDataMessage(std::move(*colors_message));
  if (!s.ok()) return s;

  *this = Pointcloud{std::move(points), std::move(colors),
                     base::TimeDelta::FromMicroseconds(message.timestamp())};

  return Status::OK();
}

}  // namespace map
}  // namespace felicia