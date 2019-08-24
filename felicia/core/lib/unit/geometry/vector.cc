#include "felicia/core/lib/unit/geometry/vector.h"

namespace felicia {

VectoriMessage VectoriToVectoriMessage(const Vectori& vector) {
  return VectorToVectorMessage<VectoriMessage>(vector);
}

VectorfMessage VectorfToVectorfMessage(const Vectorf& vector) {
  return VectorToVectorMessage<VectorfMessage>(vector);
}

VectordMessage VectordToVectordMessage(const Vectord& vector) {
  return VectorToVectorMessage<VectordMessage>(vector);
}

Vectori VectoriMessageToVectori(const VectoriMessage& message) {
  return VectorMessageToVector<int>(message);
}

Vectorf VectorfMessageToVectorf(const VectorfMessage& message) {
  return VectorMessageToVector<float>(message);
}

Vectord VectordMessageToVectord(const VectordMessage& message) {
  return VectorMessageToVector<double>(message);
}

Vector3iMessage Vector3iToVector3iMessage(const Vector3i& vector) {
  return Vector3ToVector3Message<Vector3iMessage>(vector);
}

Vector3fMessage Vector3fToVector3fMessage(const Vector3f& vector) {
  return Vector3ToVector3Message<Vector3fMessage>(vector);
}

Vector3dMessage Vector3dToVector3dMessage(const Vector3d& vector) {
  return Vector3ToVector3Message<Vector3dMessage>(vector);
}

Vector3i Vector3iMessageToVector3i(const Vector3iMessage& message) {
  return Vector3MessageToVector3<int>(message);
}

Vector3f Vector3fMessageToVector3f(const Vector3fMessage& message) {
  return Vector3MessageToVector3<float>(message);
}

Vector3d Vector3dMessageToVector3d(const Vector3dMessage& message) {
  return Vector3MessageToVector3<double>(message);
}

}  // namespace felicia