#include "felicia/core/lib/unit/geometry/vector.h"

namespace felicia {

template <typename MessageType, typename VectorType>
MessageType VectorToVectorMessage(const VectorType& vector) {
  MessageType message;
  message.set_x(vector.x());
  message.set_y(vector.y());
  return message;
}

VectorfMessage VectorfToVectorfMessage(const Vectorf& vector) {
  return VectorToVectorMessage<VectorfMessage>(vector);
}

VectordMessage VectordToVectordMessage(const Vectord& vector) {
  return VectorToVectorMessage<VectordMessage>(vector);
}

template <typename VectorType, typename MessageType>
VectorType VectorMessageToVector(const MessageType& message) {
  return {message.x(), message.y()};
}

Vectorf VectorfMessageToVectorf(const VectorfMessage& message) {
  return VectorMessageToVector<Vectorf>(message);
}

Vectord VectordMessageToVectord(const VectordMessage& message) {
  return VectorMessageToVector<Vectord>(message);
}

template <typename MessageType, typename VectorType>
MessageType Vector3ToVector3Message(const VectorType& vector) {
  MessageType message;
  message.set_x(vector.x());
  message.set_y(vector.y());
  message.set_z(vector.z());
  return message;
}

Vector3fMessage Vector3fToVector3fMessage(const Vector3f& vector) {
  return Vector3ToVector3Message<Vector3fMessage>(vector);
}

Vector3dMessage Vector3dToVector3dMessage(const Vector3d& vector) {
  return Vector3ToVector3Message<Vector3dMessage>(vector);
}

template <typename VectorType, typename MessageType>
VectorType Vector3MessageToVector3(const MessageType& message) {
  return {message.x(), message.y(), message.z()};
}

Vector3f Vector3fMessageToVector3f(const Vector3fMessage& message) {
  return Vector3MessageToVector3<Vector3f>(message);
}

Vector3d Vector3dMessageToVector3d(const Vector3dMessage& message) {
  return Vector3MessageToVector3<Vector3d>(message);
}

}  // namespace felicia