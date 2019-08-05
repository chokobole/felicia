#include "felicia/examples/slam/hecto_slam/point_container.h"

namespace felicia {

PointContainer::PointContainer() = default;

PointContainer::PointContainer(std::vector<Pointf>&& points, float factor)
    : points_(std::move(points)) {
  size_t size = points_.size();
  for (size_t i = 0; i < size; ++i) {
    points_[i] *= factor;
  }
}

void PointContainer::SetFrom(const PointContainer& other, float factor) {
  origo_ = other.origo() * factor;

  points_ = other.points_;

  size_t size = points_.size();
  for (size_t i = 0; i < size; ++i) {
    points_[i] *= factor;
  }
}

}  // namespace felicia