// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#ifndef FELICIA_CORE_CHANNEL_ROS_SERVICE_RESPONSE_H_
#define FELICIA_CORE_CHANNEL_ROS_SERVICE_RESPONSE_H_

#include "felicia/core/channel/message_receiver.h"
#include "felicia/core/channel/message_sender.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/message/ros_header.h"

namespace felicia {

namespace rpc {

template <typename T, typename SFINAE>
class Server;

}  // namespace rpc

class FEL_EXPORT RosServiceResponse {
 public:
  using HandshakeCallback = base::OnceCallback<void(std::unique_ptr<Channel>)>;

  explicit RosServiceResponse(std::unique_ptr<Channel> channel);
  ~RosServiceResponse();

  template <typename T, typename SFINAE>
  void ReceiveRequest(rpc::Server<T, SFINAE>* server,
                      HandshakeCallback callback) {
    RosServiceResponseHeader header;
    header.md5sum = server->GetServiceMD5Sum();
    header.request_type = server->GetRequestTypeName();
    header.response_type = server->GetResponseTypeName();
    header.type = server->GetServiceTypeName();

    callback_ = std::move(callback);
    channel_->SetDynamicReceiveBuffer(true);
    receiver_.set_channel(channel_.get());
    receiver_.ReceiveMessage(base::BindOnce(
        &RosServiceResponse::OnReceiveRequest, base::Unretained(this), header));
  }

  void OnReceiveRequest(const RosServiceResponseHeader& header, Status s);

  void OnResponse(bool sent_error, Status s);

 private:
  std::unique_ptr<Channel> channel_;
  MessageReceiver<RosServiceRequestHeader> receiver_;
  HandshakeCallback callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_ROS_SERVICE_RESPONSE_H_

#endif  // defined(HAS_ROS)