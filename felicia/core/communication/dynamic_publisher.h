// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_COMMUNICATION_DYNAMIC_PUBLIHSER_H_
#define FELICIA_CORE_COMMUNICATION_DYNAMIC_PUBLIHSER_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/message/dynamic_protobuf_message.h"

namespace felicia {

class DynamicPublisher : public Publisher<DynamicProtobufMessage> {
 public:
  DynamicPublisher();
  ~DynamicPublisher();

  bool ResolveType(const std::string& message_type);

  void PublishFromJson(const std::string& json_message,
                       SendMessageCallback callback = SendMessageCallback());

 private:
  std::string GetMessageTypeName() const override;

  DynamicProtobufMessage message_prototype_;

  DISALLOW_COPY_AND_ASSIGN(DynamicPublisher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_DYNAMIC_PUBLIHSER_H_