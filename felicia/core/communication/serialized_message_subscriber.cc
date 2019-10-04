#include "felicia/core/communication/serialized_message_subscriber.h"

namespace felicia {

SerializedMessageSubscriber::SerializedMessageSubscriber() = default;

SerializedMessageSubscriber::~SerializedMessageSubscriber() = default;

void SerializedMessageSubscriber::SetMessageTypeName(
    const std::string& message_type_name) {
  message_type_name_ = message_type_name;
}

std::string SerializedMessageSubscriber::GetMessageTypeName() const {
  return message_type_name_;
}

void SerializedMessageSubscriber::SetMessageImplType(
    TopicInfo::ImplType impl_type) {
  impl_type_ = impl_type;
}

TopicInfo::ImplType SerializedMessageSubscriber::GetMessageImplType() const {
  return impl_type_;
}

bool SerializedMessageSubscriber::MaybeResolveMessgaeType(
    const TopicInfo& topic_info) {
  message_type_name_ = topic_info.type_name();
  return true;
}

}  // namespace felicia