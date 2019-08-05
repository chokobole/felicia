#include "felicia/core/lib/unit/geometry/quaternion.h"

namespace felicia {

QuaternionfMessage QuaternionfToQuaternionfMessage(
    const Quaternionf& quaternion) {
  return QuaternionToQuaternionMessage<QuaternionfMessage>(quaternion);
}

QuaterniondMessage QuaterniondToQuaterniondMessage(
    const Quaterniond& quaternion) {
  return QuaternionToQuaternionMessage<QuaterniondMessage>(quaternion);
}

Quaternionf QuaternionfMessageToQuaternionf(const QuaternionfMessage& message) {
  return QuaternionMessageToQuaternion<float>(message);
}

Quaterniond QuaterniondMessageToQuaterniond(const QuaterniondMessage& message) {
  return QuaternionMessageToQuaternion<double>(message);
}

}  // namespace felicia