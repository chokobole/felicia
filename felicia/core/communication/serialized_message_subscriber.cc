#include "felicia/core/communication/serialized_message_subscriber.h"

namespace felicia {

SerializedMessageSubscriber::SerializedMessageSubscriber() = default;

SerializedMessageSubscriber::SerializedMessageSubscriber(
    const std::string& message_type_name, TopicInfo::ImplType impl_type)
    : message_type_name_(message_type_name), impl_type_(impl_type) {}

SerializedMessageSubscriber::~SerializedMessageSubscriber() = default;

#if defined(HAS_ROS)
std::string SerializedMessageSubscriber::GetMessageMD5Sum() const {
  return message_md5_sum_;
}
#endif

std::string SerializedMessageSubscriber::GetMessageTypeName() const {
  return message_type_name_;
}

}  // namespace felicia