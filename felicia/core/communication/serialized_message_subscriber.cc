#include "felicia/core/communication/serialized_message_subscriber.h"

namespace felicia {

SerializedMessageSubscriber::SerializedMessageSubscriber(
    TopicInfo::ImplType impl_type)
    : impl_type_(impl_type) {}

SerializedMessageSubscriber::~SerializedMessageSubscriber() = default;

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