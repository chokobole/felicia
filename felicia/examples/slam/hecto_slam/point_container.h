#ifndef FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_POINT_CONTAINER_H_
#define FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_POINT_CONTAINER_H_

#include "felicia/core/lib/unit/geometry/point.h"

namespace felicia {

class PointContainer {
 public:
  PointContainer();
  PointContainer(std::vector<Pointf>&& points, float factor);

  void SetFrom(const PointContainer& other, float factor);

  const std::vector<Pointf>& points() const { return points_; }

  Pointf origo() const { return origo_; }

  void set_origo(const Pointf& origo) { origo_ = origo; }

 private:
  Pointf origo_;
  std::vector<Pointf> points_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_SLAM_HECTOR_SLAM_POINT_CONTAINER_H_