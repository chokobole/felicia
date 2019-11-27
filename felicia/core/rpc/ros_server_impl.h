// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_SERVER_IMPL_H_
#define FELICIA_CORE_RPC_ROS_SERVER_IMPL_H_

#include "third_party/chromium/base/memory/scoped_refptr.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/channel/ros_service_response.h"
#include "felicia/core/message/ros_protocol.h"
#include "felicia/core/rpc/ros_serialized_service_interface.h"
#include "felicia/core/rpc/ros_service_manager.h"
#include "felicia/core/rpc/ros_util.h"
#include "felicia/core/rpc/server_interface.h"

namespace felicia {
namespace rpc {

#define FEL_ROS_SERVER                           \
  Server<T, std::enable_if_t<                    \
                IsRosServiceWrapper<T>::value || \
                std::is_base_of<RosSerializedServiceInterface, T>::value>>

template <typename T>
class FEL_ROS_SERVER : public ServerInterface {
 public:
  typedef T Service;
  typedef typename Service::Request Request;
  typedef typename Service::Response Response;
  typedef internal::RosServiceHandler<Service, Request, Response>
      ServiceHandler;
  typedef internal::RosServiceManager<Service, Request, Response>
      ServiceManager;

  Server() : service_manager_(base::MakeRefCounted<ServiceManager>()) {}
  explicit Server(std::unique_ptr<Service> service)
      : service_manager_(
            base::MakeRefCounted<ServiceManager>(std::move(service))) {}
  ~Server() override = default;

  Server& operator=(Server&& other) = default;

  Status Start() override {
    channel_ = ChannelFactory::NewChannel(ChannelDef::CHANNEL_TYPE_TCP);
    TCPChannel* tcp_channel = channel_->ToTCPChannel();
    auto status_or = tcp_channel->Listen();
    if (status_or.ok()) {
      port_ = status_or.ValueOrDie().ip_endpoint().port();
      return Status::OK();
    }
    return status_or.status();
  }

  // Non-blocking
  Status Run() override;

  Status Shutdown() override {
    channel_.reset();
    service_manager_->ClearHandlers();
    return Status::OK();
  }

  std::string GetServiceTypeName() const override {
    return service_manager_->service()->GetServiceTypeName();
  }

  std::string GetServiceMD5Sum() const {
    return service_manager_->service()->GetServiceMD5Sum();
  }

  std::string GetRequestTypeName() const {
    return service_manager_->service()->GetRequestTypeName();
  }

  std::string GetResponseTypeName() const {
    return service_manager_->service()->GetResponseTypeName();
  }

 protected:
  friend class felicia::RosServiceResponse;

  void DoAcceptLoop();
  void OnAccept(StatusOr<std::unique_ptr<TCPChannel>> status_or);
  void OnRosServiceHandshake(std::unique_ptr<Channel> client_channel,
                             bool persistent);

  scoped_refptr<ServiceManager> service_manager_;
  std::unique_ptr<Channel> channel_;
};

template <typename T>
Status FEL_ROS_SERVER::Run() {
  DoAcceptLoop();
  return Status::OK();
}

template <typename T>
void FEL_ROS_SERVER::DoAcceptLoop() {
  TCPChannel* tcp_channel = channel_->ToTCPChannel();
  tcp_channel->AcceptOnceIntercept(
      base::BindRepeating(&Server::OnAccept, base::Unretained(this)));
}

template <typename T>
void FEL_ROS_SERVER::OnAccept(StatusOr<std::unique_ptr<TCPChannel>> status_or) {
  if (status_or.ok()) {
    if (IsUsingRosProtocol(service_info_.service())) {
      // It deletes itself when handshake is completed.
      RosServiceResponse* service_response =
          new RosServiceResponse(std::move(status_or).ValueOrDie());
      service_response->ReceiveRequest(
          this, base::BindOnce(&Server::OnRosServiceHandshake,
                               base::Unretained(this)));
    } else {
      service_manager_->AddHandler(std::make_unique<ServiceHandler>(
          service_manager_, std::move(status_or).ValueOrDie(), false));
    }
  }
  DoAcceptLoop();
}

template <typename T>
void FEL_ROS_SERVER::OnRosServiceHandshake(
    std::unique_ptr<Channel> client_channel, bool persistent) {
  std::unique_ptr<TCPChannel> client_tcp_channel;
  client_tcp_channel.reset(
      reinterpret_cast<TCPChannel*>(client_channel.release()));
  service_manager_->AddHandler(std::make_unique<ServiceHandler>(
      service_manager_, std::move(client_tcp_channel), true, persistent));
}

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_SERVER_IMPL_H_

#endif  // defined(HAS_ROS)