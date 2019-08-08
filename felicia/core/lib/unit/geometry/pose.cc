#include "felicia/core/lib/unit/geometry/pose.h"

namespace felicia {

PosefMessage PosefToPosefMessage(const Posef& pose) {
  return PoseToPoseMessage<PosefMessage, PointfMessage>(pose);
}

PosedMessage PosedToPosedMessage(const Posed& pose) {
  return PoseToPoseMessage<PosedMessage, PointdMessage>(pose);
}

Posef PosefMessageToPosef(const PosefMessage& message) {
  return PoseMessageToPose<float>(message);
}

Posed PosedMessageToPosed(const PosedMessage& message) {
  return PoseMessageToPose<double>(message);
}

PosefWithTimestampMessage PosefToPosefWithTimestampMessage(
    const Posef& pose, base::TimeDelta timestamp) {
  return PoseToPoseWithTimestampMessage<PosefWithTimestampMessage,
                                        PointfMessage>(pose, timestamp);
}

PosedWithTimestampMessage PosedToPosedWithTimestampMessage(
    const Posed& pose, base::TimeDelta timestamp) {
  return PoseToPoseWithTimestampMessage<PosedWithTimestampMessage,
                                        PointdMessage>(pose, timestamp);
}

Posef PosefWithTimestampMessageToPosef(
    const PosefWithTimestampMessage& message) {
  return PoseWithTimestampMessageToPose<float>(message);
}

Posed PosedWithTimestampMessageToPosed(
    const PosedWithTimestampMessage& message) {
  return PoseWithTimestampMessageToPose<double>(message);
}

Pose3fMessage Pose3fToPose3fMessage(const Pose3f& pose) {
  return Pose3ToPose3Message<Pose3fMessage, Point3fMessage, QuaternionfMessage>(
      pose);
}

Pose3dMessage Pose3dToPose3dMessage(const Pose3d& pose) {
  return Pose3ToPose3Message<Pose3dMessage, Point3dMessage, QuaterniondMessage>(
      pose);
}

Pose3f Pose3fMessageToPose3f(const Pose3fMessage& message) {
  return Pose3MessageToPose3<float>(message);
}

Pose3d Pose3dMessageToPose3d(const Pose3dMessage& message) {
  return Pose3MessageToPose3<double>(message);
}

Pose3fWithTimestampMessage Pose3fToPose3fWithTimestampMessage(
    const Pose3f& pose, base::TimeDelta timestamp) {
  return Pose3ToPose3WithTimestampMessage<Pose3fWithTimestampMessage,
                                          Point3fMessage, QuaternionfMessage>(
      pose, timestamp);
}

Pose3dWithTimestampMessage Pose3dToPose3dWithTimestampMessage(
    const Pose3d& pose, base::TimeDelta timestamp) {
  return Pose3ToPose3WithTimestampMessage<Pose3dWithTimestampMessage,
                                          Point3dMessage, QuaterniondMessage>(
      pose, timestamp);
}

Pose3f Pose3fWithTimestampMessageToPose3f(
    const Pose3fWithTimestampMessage& message) {
  return Pose3WithTimestampMessageToPose3<float>(message);
}

Pose3d Pose3dWithTimestampMessageToPose3d(
    const Pose3dWithTimestampMessage& message) {
  return Pose3WithTimestampMessageToPose3<double>(message);
}

}  // namespace felicia