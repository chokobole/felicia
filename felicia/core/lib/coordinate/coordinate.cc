#include "felicia/core/lib/coordinate/coordinate.h"

namespace felicia {

Coordinate::Coordinate() : coordinate_system_(COORDINATE_SYSTEM_IMAGE) {}

Coordinate::Coordinate(Coordinate::CoordinateSystem coordinate_system)
    : coordinate_system_(coordinate_system) {}

Coordinate::Coordinate(const Coordinate& other) = default;
Coordinate& Coordinate::operator=(const Coordinate& other) = default;

Coordinate::CoordinateSystem Coordinate::coordinate_system() const {
  return coordinate_system_;
}

}  // namespace felicia