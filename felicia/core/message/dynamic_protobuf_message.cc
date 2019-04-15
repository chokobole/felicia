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

  return *this;
}

DynamicProtobufMessage::~DynamicProtobufMessage() { delete message_; }

void DynamicProtobufMessage::Reset(::google::protobuf::Message* message) {
  message_ = message;
}

std::string DynamicProtobufMessage::GetTypeName() const {
  if (message_) return message_->GetTypeName();
  return ::base::EmptyString();
}

std::string DynamicProtobufMessage::DebugString() const {
  if (!message_) return ::base::EmptyString();
  return message_->DebugString();
}

Status DynamicProtobufMessage::MessageToJsonString(std::string* text) const {
  ::google::protobuf::util::Status status =
      ::google::protobuf::util::MessageToJsonString(*message_, text);
  return Status(static_cast<felicia::error::Code>(status.error_code()),
                status.error_message().as_string());
}

bool DynamicProtobufMessage::SerializeToString(std::string* text) const {
  return message_->SerializeToString(text);
}

bool DynamicProtobufMessage::ParseFromArray(const char* data, size_t size) {
  return message_->ParseFromArray(data, size);
}

}  // namespace felicia