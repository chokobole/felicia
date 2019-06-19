#include "felicia/core/lib/unit/geometry/geometry_util.h"

namespace felicia {

Vec3fMessage EigenVec3fToVec3fMessage(const ::Eigen::Vector3f& vec) {
  Vec3fMessage message;
  message.set_x(vec.x());
  message.set_y(vec.y());
  message.set_z(vec.z());
  return message;
}

QuarternionMessage EigenQuarternionfToQuarternionMessage(
    const ::Eigen::Quaternionf& quarternion) {
  QuarternionMessage message;
  message.set_w(quarternion.w());
  message.set_x(quarternion.x());
  message.set_y(quarternion.y());
  message.set_z(quarternion.z());
  return message;
}

Point3fMessage Point3fToPoint3fMessage(const Point3f& point) {
  Point3fMessage message;
  message.set_x(point.x());
  message.set_y(point.y());
  message.set_z(point.z());
  return message;
}

}  // namespace felicia