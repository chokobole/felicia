#include "felicia/core/communication/serialized_message_publisher.h"

namespace felicia {

SerializedMessagePublisher::SerializedMessagePublisher() = default;

SerializedMessagePublisher::SerializedMessagePublisher(
    const std::string& message_type_name, TopicInfo::ImplType impl_type)
    : message_type_name_(message_type_name), impl_type_(impl_type) {}

SerializedMessagePublisher::~SerializedMessagePublisher() = default;

void SerializedMessagePublisher::SetMessageTypeName(
    const std::string& message_type_name) {
  message_type_name_ = message_type_name;
}

std::string SerializedMessagePublisher::GetMessageTypeName() const {
  return message_type_name_;
}

void SerializedMessagePublisher::SetMessageImplType(
    TopicInfo::ImplType impl_type) {
  impl_type_ = impl_type;
}

TopicInfo::ImplType SerializedMessagePublisher::GetMessageImplType() const {
  return impl_type_;
}

void SerializedMessagePublisher::PublishFromSerialized(
    const std::string& serialized, SendMessageCallback callback) {
  message_.set_serialized(serialized);
  Publisher<SerializedMessage>::Publish(message_, callback);
}

void SerializedMessagePublisher::PublishFromSerialized(
    std::string&& serialized, SendMessageCallback callback) {
  message_.set_serialized(std::move(serialized));
  Publisher<SerializedMessage>::Publish(message_, callback);
}

MessageIOError SerializedMessagePublisher::SerializeToString(
    SerializedMessage* message, std::string* serialized) {
  *serialized = std::move(*message).serialized();
  return MessageIOError::OK;
}

}  // namespace felicia