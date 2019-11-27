// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_SERVICE_HANDLER_H_
#define FELICIA_CORE_RPC_ROS_SERVICE_HANDLER_H_

#include <memory>

#include "third_party/chromium/base/memory/scoped_refptr.h"

#include "felicia/core/channel/message_receiver.h"
#include "felicia/core/channel/message_sender.h"
#include "felicia/core/channel/tcp_channel.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/message/ros_rpc_header.h"

namespace felicia {
namespace rpc {
namespace internal {

template <typename Service, typename Request, typename Response>
class RosServiceManager;

template <typename Service, typename Request, typename Response>
class RosServiceHandler {
 public:
  typedef RosServiceManager<Service, Request, Response> ServiceManager;

  RosServiceHandler(scoped_refptr<ServiceManager> service_manager,
                    std::unique_ptr<TCPChannel> channel, bool use_ros_protocol,
                    bool persistent = true)
      : service_manager_(service_manager),
        channel_(std::move(channel)),
        use_ros_protocol_(use_ros_protocol),
        persistent_(persistent) {
    channel_->SetDynamicSendBuffer(true);
    channel_->SetDynamicReceiveBuffer(true);
    receiver_.set_channel(channel_.get());
    ReceiveRequest();
  }

 private:
  void ReceiveRequest();
  void OnReceiveRequest(Status s);
  void SendResponse(bool ok);
  void OnSendResponse(Status s);
  void OnHandleRequest(Status s);

  Response response_;
  scoped_refptr<ServiceManager> service_manager_;
  std::unique_ptr<TCPChannel> channel_;
  MessageReceiver<Request> receiver_;
  bool use_ros_protocol_;
  bool persistent_;
};

template <typename Service, typename Request, typename Response>
void RosServiceHandler<Service, Request, Response>::ReceiveRequest() {
  receiver_.ReceiveMessage(base::BindOnce(
      &RosServiceHandler<Service, Request, Response>::OnReceiveRequest,
      base::Unretained(this)));
}

template <typename Service, typename Request, typename Response>
void RosServiceHandler<Service, Request, Response>::OnReceiveRequest(Status s) {
  if (s.ok()) {
    service_manager_->service()->Handle(
        &receiver_.message(), &response_,
        base::BindOnce(
            &RosServiceHandler<Service, Request, Response>::OnHandleRequest,
            base::Unretained(this)));
  } else {
    LOG(ERROR) << s;
    service_manager_->RemoveHandler(this);
  }
}

template <typename Service, typename Request, typename Response>
void RosServiceHandler<Service, Request, Response>::SendResponse(bool ok) {
  MessageSender<Response> sender(channel_.get());
  RosRpcHeader header;
  if (use_ros_protocol_) {
    header.set_ok(ok);
    sender.set_attach_header_callback(
        base::BindOnce(&RosRpcHeader::AttachHeader, base::Unretained(&header)));
  }
  sender.SendMessage(
      response_,
      base::BindOnce(
          &RosServiceHandler<Service, Request, Response>::OnSendResponse,
          base::Unretained(this)));
}

template <typename Service, typename Request, typename Response>
void RosServiceHandler<Service, Request, Response>::OnSendResponse(Status s) {
  if (s.ok()) {
    if (persistent_) {
      ReceiveRequest();
    } else {
      service_manager_->RemoveHandler(this);
    }
  } else {
    LOG(ERROR) << s;
    service_manager_->RemoveHandler(this);
  }
}

template <typename Service, typename Request, typename Response>
void RosServiceHandler<Service, Request, Response>::OnHandleRequest(Status s) {
  LOG_IF(ERROR, !s.ok()) << s;
  SendResponse(s.ok());
}

}  // namespace internal
}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_SERVICE_HANDLER_H_

#endif  // defined(HAS_ROS)