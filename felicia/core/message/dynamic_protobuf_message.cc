// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/message/dynamic_protobuf_message.h"

#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/message/protobuf_util.h"

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
  delete message_;
  message_ = nullptr;

  if (other.message_) {
    message_ = other.message_->New();
    message_->CopyFrom(*other.message_);
  }

  return *this;
}

DynamicProtobufMessage::DynamicProtobufMessage(
    DynamicProtobufMessage&& other) noexcept {
  if (other.message_) {
    message_ = other.message_;
    other.message_ = nullptr;
  }
}

DynamicProtobufMessage& DynamicProtobufMessage::operator=(
    DynamicProtobufMessage&& other) {
  delete message_;
  message_ = nullptr;

  if (other.message_) {
    message_ = other.message_;
    other.message_ = nullptr;
  }

  return *this;
}

DynamicProtobufMessage::~DynamicProtobufMessage() { delete message_; }

google::protobuf::Message* DynamicProtobufMessage::message() {
  return message_;
}

const google::protobuf::Message* DynamicProtobufMessage::message() const {
  return message_;
}

void DynamicProtobufMessage::Reset(google::protobuf::Message* message) {
  DCHECK(message);
  delete message_;
  message_ = message;
}

std::string DynamicProtobufMessage::GetTypeName() const {
  if (message_) return message_->GetTypeName();
  return base::EmptyString();
}

std::string DynamicProtobufMessage::ToString() const {
  if (!message_) return base::EmptyString();
  return protobuf::ProtobufMessageToString(*message_);
}

std::string DynamicProtobufMessage::DebugString() const {
  if (!message_) return base::EmptyString();
  return message_->DebugString();
}

Status DynamicProtobufMessage::MessageToJsonString(std::string* text) const {
  if (!message_) return errors::NotFound("message is null.");
  google::protobuf::util::Status status =
      google::protobuf::util::MessageToJsonString(*message_, text);
  return Status(static_cast<felicia::error::Code>(status.error_code()),
                status.error_message().as_string());
}

bool DynamicProtobufMessage::SerializeToString(std::string* text) const {
  if (!message_) return false;
  return message_->SerializeToString(text);
}

bool DynamicProtobufMessage::ParseFromArray(const char* data, size_t size) {
  if (!message_) return false;
  return message_->ParseFromArray(data, size);
}

}  // namespace felicia