// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_PUBLIHSER_H_
#define FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_PUBLIHSER_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/message/serialized_message.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {

class SerializedMessagePublisher : public Publisher<SerializedMessage> {
 public:
  SerializedMessagePublisher();
  explicit SerializedMessagePublisher(
      const std::string& message_type_name,
      TopicInfo::ImplType impl_type = TopicInfo::PROTOBUF);
  ~SerializedMessagePublisher();

  void PublishFromSerialized(
      const std::string& serialized,
      SendMessageCallback callback = SendMessageCallback());

  void PublishFromSerialized(
      std::string&& serialized,
      SendMessageCallback callback = SendMessageCallback());

 protected:
#if defined(HAS_ROS)
  std::string GetMessageMD5Sum() const override;
  std::string GetMessageDefinition() const override;
#endif  // defined(HAS_ROS)
  std::string GetMessageTypeName() const override;
  TopicInfo::ImplType GetMessageImplType() const override;

  MessageIOError SerializeToString(SerializedMessage* message,
                                   std::string* serialized) override;

  SerializedMessage message_;
#if defined(HAS_ROS)
  std::string message_md5_sum_;
  std::string message_definition_;
#endif  // defined(HAS_ROS)
  std::string message_type_name_;
  TopicInfo::ImplType impl_type_ = TopicInfo::PROTOBUF;

  DISALLOW_COPY_AND_ASSIGN(SerializedMessagePublisher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_SERIALIZED_MESSAGE_PUBLIHSER_H_