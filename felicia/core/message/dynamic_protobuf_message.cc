#include "felicia/core/message/dynamic_protobuf_message.h"

#include "third_party/chromium/base/strings/string_util.h"

namespace felicia {

DynamicProtobufMessage::DynamicProtobufMessage() = default;

DynamicProtobufMessage::DynamicProtobufMessage(
    const DynamicProtobufMessage& other) {
  if (other.message_) {
    message_ = other.message_->New();
    message_->CopyFrom(*other.message_);
  }
}

DynamicProtobufMessage& DynamicProtobufMessage::operator=(
    const DynamicProtobufMessage& other) {
  if (other.message_) {
    message_ = other.message_->New();
    message_->CopyFrom(*other.message_);
  }
}

DynamicProtobufMessage::~DynamicProtobufMessage() { delete message_; }

void DynamicProtobufMessage::Reset(::google::protobuf::Message* message) {
  message_ = message;
}

std::string DynamicProtobufMessage::DebugString() const {
  if (!message_) return ::base::EmptyString();
  return message_->DebugString();
}

bool DynamicProtobufMessage::SerializeToString(std::string* text) const {
  return message_->SerializeToString(text);
}

bool DynamicProtobufMessage::ParseFromArray(const char* data, size_t size) {
  return message_->ParseFromArray(data, size);
}

}  // namespace felicia