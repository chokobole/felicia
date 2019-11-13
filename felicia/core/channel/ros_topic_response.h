// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#ifndef FELICIA_CORE_CHANNEL_ROS_TOPIC_RESPONSE_H_
#define FELICIA_CORE_CHANNEL_ROS_TOPIC_RESPONSE_H_

#include "felicia/core/channel/message_receiver.h"
#include "felicia/core/channel/message_sender.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/message/ros_header.h"
#include "felicia/core/message/ros_protocol.h"

namespace felicia {

template <typename MessageTy>
class Publisher;

class FEL_EXPORT RosTopicResponse {
 public:
  using HandshakeCallback = base::OnceCallback<void(std::unique_ptr<Channel>)>;

  explicit RosTopicResponse(std::unique_ptr<Channel> channel);
  ~RosTopicResponse();

  template <typename MessageTy>
  void ReceiveRequest(Publisher<MessageTy>* publisher,
                      HandshakeCallback callback) {
    RosTopicResponseHeader header;
    ConsumeRosProtocol(publisher->topic_info_.topic(), &header.topic);
    header.md5sum = publisher->GetMessageMD5Sum();
    header.type = publisher->GetMessageTypeName();
    header.message_definition = publisher->GetMessageDefinition();
    header.latching = "0";

    callback_ = std::move(callback);
    channel_->SetDynamicReceiveBuffer(true);
    receiver_.set_channel(channel_.get());
    receiver_.ReceiveMessage(base::BindOnce(&RosTopicResponse::OnReceiveRequest,
                                            base::Unretained(this), header));
  }

  void OnReceiveRequest(const RosTopicResponseHeader& header, Status s);

  void OnResponse(bool sent_error, Status s);

 private:
  std::unique_ptr<Channel> channel_;
  MessageReceiver<RosTopicRequestHeader> receiver_;
  HandshakeCallback callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_ROS_TOPIC_RESPONSE_H_

#endif  // defined(HAS_ROS)