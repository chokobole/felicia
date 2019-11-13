// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/message/serialized_message.h"

namespace felicia {

SerializedMessage::SerializedMessage() = default;

SerializedMessage::SerializedMessage(const SerializedMessage& other) = default;

SerializedMessage& SerializedMessage::operator=(
    const SerializedMessage& other) = default;

SerializedMessage::SerializedMessage(SerializedMessage&& other) noexcept
    : serialized_(std::move(other.serialized_)) {}

SerializedMessage& SerializedMessage::operator=(
    SerializedMessage&& other) noexcept {
  serialized_ = std::move(other.serialized_);
  return *this;
}

SerializedMessage::~SerializedMessage() = default;

void SerializedMessage::set_serialized(const std::string& serialized) {
  serialized_ = serialized;
}

void SerializedMessage::set_serialized(std::string&& serialized) {
  serialized_ = std::move(serialized);
}

const std::string& SerializedMessage::serialized() const& {
  return serialized_;
}

std::string&& SerializedMessage::serialized() && {
  return std::move(serialized_);
}

bool SerializedMessage::SerializeToString(std::string* text) const& {
  *text = serialized_;
  return true;
}

bool SerializedMessage::SerializeToString(std::string* text) && {
  *text = std::move(serialized_);
  return true;
}

std::ostream& operator<<(std::ostream& os, const SerializedMessage& m) {
  os << m.serialized();
  return os;
}

}  // namespace felicia