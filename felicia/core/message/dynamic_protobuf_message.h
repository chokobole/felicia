// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MESSAGE_DYNAMIC_PROTOBUF_MESSAGE_H_
#define FELICIA_CORE_MESSAGE_DYNAMIC_PROTOBUF_MESSAGE_H_

#include "google/protobuf/message.h"
#include "google/protobuf/util/json_util.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class FEL_EXPORT DynamicProtobufMessage {
 public:
  DynamicProtobufMessage();
  DynamicProtobufMessage(const DynamicProtobufMessage& other);
  DynamicProtobufMessage& operator=(const DynamicProtobufMessage& other);
  DynamicProtobufMessage(DynamicProtobufMessage&& other) noexcept;
  DynamicProtobufMessage& operator=(DynamicProtobufMessage&& other);
  ~DynamicProtobufMessage();

  google::protobuf::Message* message();
  const google::protobuf::Message* message() const;

  void Reset(google::protobuf::Message* message);

  std::string ToString() const;
  std::string DebugString() const;

  std::string GetTypeName() const;

  Status MessageToJsonString(std::string* text) const;
  bool SerializeToString(std::string* text) const;
  bool ParseFromArray(const char* data, size_t size);

 private:
  google::protobuf::Message* message_ = nullptr;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_DYNAMIC_PROTOBUF_MESSAGE_H_