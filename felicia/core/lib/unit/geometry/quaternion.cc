#include "felicia/core/lib/unit/geometry/quaternion.h"

namespace felicia {

template <typename MessageType, typename QuaternionType>
MessageType QuaternionToQuaternionMessage(const QuaternionType& quaternion) {
  MessageType message;
  message.set_w(quaternion.w());
  message.set_x(quaternion.x());
  message.set_y(quaternion.y());
  message.set_z(quaternion.z());
  return message;
}

QuaternionfMessage QuaternionfToQuaternionfMessage(
    const Quaternionf& quaternion) {
  return QuaternionToQuaternionMessage<QuaternionfMessage>(quaternion);
}

QuaterniondMessage QuaterniondToQuaterniondMessage(
    const Quaterniond& quaternion) {
  return QuaternionToQuaternionMessage<QuaterniondMessage>(quaternion);
}

template <typename QuaternionType, typename MessageType>
QuaternionType QuaternionMessageToQuaternion(const MessageType& message) {
  return {message.x(), message.y(), message.z(), message.w()};
}

Quaternionf QuaternionfMessageToQuaternionf(const QuaternionfMessage& message) {
  return QuaternionMessageToQuaternion<Quaternionf>(message);
}

Quaterniond QuaterniondMessageToQuaterniond(const QuaterniondMessage& message) {
  return QuaternionMessageToQuaternion<Quaterniond>(message);
}

}  // namespace felicia