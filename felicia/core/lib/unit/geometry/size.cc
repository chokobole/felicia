#include "felicia/core/lib/unit/geometry/size.h"

namespace felicia {

SizeiMessage SizeiToSizeiMessage(const Sizei& size) {
  return SizeToSizeMessage<SizeiMessage>(size);
}

SizefMessage SizefToSizefMessage(const Sizef& size) {
  return SizeToSizeMessage<SizefMessage>(size);
}

SizedMessage SizedToSizedMessage(const Sized& size) {
  return SizeToSizeMessage<SizedMessage>(size);
}

Sizei SizeiMessageToSizei(const SizeiMessage& message) {
  return SizeMessageToSize<int>(message);
}

Sizef SizefMessageToSizef(const SizefMessage& message) {
  return SizeMessageToSize<float>(message);
}

Sized SizedMessageToSized(const SizedMessage& message) {
  return SizeMessageToSize<double>(message);
}

}  // namespace felicia