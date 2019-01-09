#include "felicia/core/communication/message.h"

namespace felicia {

Message::Message() : type_(MESSAGE_TYPE_UNINTIALIZED) {}

Message::Message(const google::protobuf::Message& protobuf_message)
    : type_(MESSAGE_TYPE_PROTOBUF) {
  set_protobuf_message(protobuf_message);
}

Message::Message(const Message& message) {
  if (message.IsProtobufMessage()) {
    set_protobuf_message(*message.protobuf_message());
  } else {
    type_ = MESSAGE_TYPE_UNINTIALIZED;
  }
}

Message::~Message() { reset(); }

void Message::reset() {
  if (IsProtobufMessage()) {
    delete value_.protobuf_message_;
  }
  type_ = MESSAGE_TYPE_UNINTIALIZED;
}

void Message::operator=(const Message& message) {
  reset();
  if (message.IsProtobufMessage()) {
    set_protobuf_message(*message.protobuf_message());
  }
}

void Message::set_protobuf_message(
    const google::protobuf::Message& protobuf_message) {
  value_ = protobuf_message.New();
  value_.protobuf_message_->CopyFrom(protobuf_message);
  type_ = MESSAGE_TYPE_PROTOBUF;
}

void Message::Clear() {
  if (IsProtobufMessage()) {
    value_.protobuf_message_->Clear();
  }
}

std::string Message::ToSerealizedString() const {
  std::string ret;
  if (IsProtobufMessage()) {
    value_.protobuf_message_->SerializeToString(&ret);
  }
  return ret;
}

Message Message::ParseFromString(const std::string& data) const {
  if (IsProtobufMessage()) {
    value_.protobuf_message_->ParseFromString(data);
  }
  return *this;
}

}  // namespace felicia