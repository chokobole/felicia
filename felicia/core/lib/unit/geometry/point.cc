#include "felicia/core/lib/unit/geometry/point.h"

namespace felicia {

PointfMessage PointfToPointfMessage(const Pointf& point) {
  return PointToPointMessage<PointfMessage>(point);
}

PointdMessage PointdToPointdMessage(const Pointd& point) {
  return PointToPointMessage<PointdMessage>(point);
}

Pointf PointfMessageToPointf(const PointfMessage& message) {
  return PointMessageToPoint<float>(message);
}

Pointd PointdMessageToPointd(const PointdMessage& message) {
  return PointMessageToPoint<double>(message);
}

Point3fMessage Point3fToPoint3fMessage(const Point3f& point) {
  return Point3ToPoint3Message<Point3fMessage>(point);
}

Point3dMessage Point3dToPoint3dMessage(const Point3d& point) {
  return Point3ToPoint3Message<Point3dMessage>(point);
}

Point3f Point3fMessageToPoint3f(const Point3fMessage& message) {
  return Point3MessageToPoint3<float>(message);
}

Point3d Point3dMessageToPoint3d(const Point3dMessage& message) {
  return Point3MessageToPoint3<double>(message);
}

}  // namespace felicia