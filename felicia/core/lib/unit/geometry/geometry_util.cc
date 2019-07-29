#include "felicia/core/lib/unit/geometry/geometry_util.h"

namespace felicia {

Vec3fMessage EigenVec3fToVec3fMessage(const ::Eigen::Vector3f& vec) {
  Vec3fMessage message;
  message.set_x(vec.x());
  message.set_y(vec.y());
  message.set_z(vec.z());
  return message;
}

Vec3dMessage EigenVec3dToVec3dMessage(const ::Eigen::Vector3d& vec) {
  Vec3dMessage message;
  message.set_x(vec.x());
  message.set_y(vec.y());
  message.set_z(vec.z());
  return message;
}

QuarternionfMessage EigenQuarternionfToQuarternionfMessage(
    const ::Eigen::Quaternionf& quarternion) {
  QuarternionfMessage message;
  message.set_w(quarternion.w());
  message.set_x(quarternion.x());
  message.set_y(quarternion.y());
  message.set_z(quarternion.z());
  return message;
}

QuarterniondMessage EigenQuarterniondToQuarterniondMessage(
    const ::Eigen::Quaterniond& quarternion) {
  QuarterniondMessage message;
  message.set_w(quarternion.w());
  message.set_x(quarternion.x());
  message.set_y(quarternion.y());
  message.set_z(quarternion.z());
  return message;
}

PointfMessage PointfToPointfMessage(const Pointf& point) {
  PointfMessage message;
  message.set_x(point.x());
  message.set_y(point.y());
  return message;
}

PointdMessage PointdToPointdMessage(const Pointd& point) {
  PointdMessage message;
  message.set_x(point.x());
  message.set_y(point.y());
  return message;
}

Point3fMessage Point3fToPoint3fMessage(const Point3f& point) {
  Point3fMessage message;
  message.set_x(point.x());
  message.set_y(point.y());
  message.set_z(point.z());
  return message;
}

Point3dMessage Point3dToPoint3dMessage(const Point3d& point) {
  Point3dMessage message;
  message.set_x(point.x());
  message.set_y(point.y());
  message.set_z(point.z());
  return message;
}

PosefMessage PosefToPosefMessage(const Posef& pose) {
  PosefMessage message;
  message.set_x(pose.x());
  message.set_y(pose.y());
  message.set_theta(pose.theta());
  return message;
}

PosedMessage PosefToPosedMessage(const Posed& pose) {
  PosedMessage message;
  message.set_x(pose.x());
  message.set_y(pose.y());
  message.set_theta(pose.theta());
  return message;
}

Pose3fMessage Pose3fToPose3fMessage(const Pose3f& pose) {
  Pose3fMessage message;
  message.set_x(pose.x());
  message.set_y(pose.y());
  message.set_z(pose.z());
  message.set_theta(pose.theta());
  return message;
}

Pose3dMessage Pose3dToPose3dMessage(const Pose3d& pose) {
  Pose3dMessage message;
  message.set_x(pose.x());
  message.set_y(pose.y());
  message.set_z(pose.z());
  message.set_theta(pose.theta());
  return message;
}

}  // namespace felicia