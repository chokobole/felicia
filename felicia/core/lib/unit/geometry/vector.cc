#include "felicia/core/lib/unit/geometry/vector.h"

namespace felicia {

VectorfMessage VectorfToVectorfMessage(const Vectorf& vector) {
  return VectorToVectorMessage<VectorfMessage>(vector);
}

VectordMessage VectordToVectordMessage(const Vectord& vector) {
  return VectorToVectorMessage<VectordMessage>(vector);
}

Vectorf VectorfMessageToVectorf(const VectorfMessage& message) {
  return VectorMessageToVector<float>(message);
}

Vectord VectordMessageToVectord(const VectordMessage& message) {
  return VectorMessageToVector<double>(message);
}

Vector3fMessage Vector3fToVector3fMessage(const Vector3f& vector) {
  return Vector3ToVector3Message<Vector3fMessage>(vector);
}

Vector3dMessage Vector3dToVector3dMessage(const Vector3d& vector) {
  return Vector3ToVector3Message<Vector3dMessage>(vector);
}

Vector3f Vector3fMessageToVector3f(const Vector3fMessage& message) {
  return Vector3MessageToVector3<float>(message);
}

Vector3d Vector3dMessageToVector3d(const Vector3dMessage& message) {
  return Vector3MessageToVector3<double>(message);
}

}  // namespace felicia