// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#ifndef FELICIA_CORE_CHANNEL_ROS_TOPIC_REQUEST_H_
#define FELICIA_CORE_CHANNEL_ROS_TOPIC_REQUEST_H_

#include "felicia/core/channel/message_receiver.h"
#include "felicia/core/channel/message_sender.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/message/ros_header.h"
#include "felicia/core/message/ros_protocol.h"

namespace felicia {

template <typename MessageTy>
class Subscriber;

class FEL_EXPORT RosTopicRequest {
 public:
  RosTopicRequest();
  ~RosTopicRequest();

  void Reset();

  template <typename MessageTy>
  void Request(Subscriber<MessageTy>* subscriber, StatusOnceCallback callback) {
    channel_ = subscriber->channel_.get();
    callback_ = std::move(callback);
    RosTopicRequestHeader header;
    ConsumeRosProtocol(subscriber->topic_info_.topic(), &header.topic);
    header.md5sum = subscriber->GetMessageMD5Sum();
    header.callerid = subscriber->topic_info_.ros_node_name();
    header.type = subscriber->GetMessageTypeName();
    header.tcp_nodelay = "1";
    MessageSender<RosTopicRequestHeader> sender(channel_);
    channel_->SetDynamicSendBuffer(true);
    sender.SendMessage(header, base::BindOnce(&RosTopicRequest::OnRequest,
                                              base::Unretained(this)));

    channel_->SetDynamicReceiveBuffer(true);
    receiver_.set_channel(channel_);
    receiver_.ReceiveMessage(base::BindOnce(&RosTopicRequest::OnReceiveResponse,
                                            base::Unretained(this), header));
  }

 private:
  void OnRequest(Status s);

  void OnReceiveResponse(const RosTopicRequestHeader& header, Status s);

  Channel* channel_;
  Header header_;
  MessageReceiver<RosTopicResponseHeader> receiver_;
  StatusOnceCallback callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_ROS_TOPIC_REQUEST_H_

#endif  // defined(HAS_ROS)