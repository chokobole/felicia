#include "felicia/core/lib/unit/geometry/pose.h"

namespace felicia {

PosefMessage PosefToPosefMessage(const Posef& pose) {
  PosefMessage message;
  *message.mutable_point() = PointfToPointfMessage(pose.point());
  message.set_theta(pose.theta());
  return message;
}

PosedMessage PosedToPosedMessage(const Posed& pose) {
  PosedMessage message;
  *message.mutable_point() = PointdToPointdMessage(pose.point());
  message.set_theta(pose.theta());
  return message;
}

Posef PosefMessageToPosef(const PosefMessage& message) {
  return {PointfMessageToPointf(message.point()), message.theta()};
}

Posed PosedMessageToPosed(const PosedMessage& message) {
  return {PointdMessageToPointd(message.point()), message.theta()};
}

Pose3fMessage Pose3fToPose3fMessage(const Pose3f& pose) {
  Pose3fMessage message;
  *message.mutable_point() = Point3fToPoint3fMessage(pose.point());
  *message.mutable_orientation() =
      QuaternionfToQuaternionfMessage(pose.orientation());
  return message;
}

Pose3dMessage Pose3dToPose3dMessage(const Pose3d& pose) {
  Pose3dMessage message;
  *message.mutable_point() = Point3dToPoint3dMessage(pose.point());
  *message.mutable_orientation() =
      QuaterniondToQuaterniondMessage(pose.orientation());
  return message;
}

Pose3f Pose3fMessageToPose3f(const Pose3fMessage& message) {
  return {Point3fMessageToPoint3f(message.point()),
          QuaternionfMessageToQuaternionf(message.orientation())};
}

Pose3d Pose3dMessageToPose3d(const Pose3dMessage& message) {
  return {Point3dMessageToPoint3d(message.point()),
          QuaterniondMessageToQuaterniond(message.orientation())};
}

}  // namespace felicia