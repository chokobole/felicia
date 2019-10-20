#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_CLIENT_IMPL_H_
#define FELICIA_CORE_RPC_ROS_CLIENT_IMPL_H_

#include <ros/service_traits.h>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/channel/message_receiver.h"
#include "felicia/core/channel/message_sender.h"
#include "felicia/core/channel/ros_service_request.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/message/ros_protocol.h"
#include "felicia/core/message/ros_rpc_header.h"
#include "felicia/core/rpc/client_interface.h"
#include "felicia/core/rpc/ros_util.h"

namespace felicia {
namespace rpc {

#define FEL_ROS_CLIENT Client<T, std::enable_if_t<IsRosService<T>::value>>

template <typename T>
class FEL_ROS_CLIENT : public ClientInterface {
 public:
  typedef T RosService;
  typedef typename RosService::Request Request;
  typedef typename RosService::Response Response;

  Client() = default;
  ~Client() override = default;

  void Connect(const IPEndPoint& ip_endpoint,
               StatusOnceCallback callback) override;

  Status Run() override;

  Status Shutdown() override {
    channel_.reset();
    service_request_.Reset();
    connect_callback_.Reset();
    call_callback_.Reset();
    return Status::OK();
  }

  void Call(const Request* request, Response* response,
            StatusOnceCallback callback);

 protected:
  friend class felicia::RosServiceRequest;

  void OnConnect(const Status& s);
  void OnRosServiceHandshake(const Status& s);
  // SendRequest if it is connected.
  void MaybeSendRequest();
  void SendRequest();
  void OnSendRequest(const Status& s);
  void ReceiveResponse();
  void OnReceiveResponse(const Status& s);

  virtual std::string GetServiceDataType() const {
    return ros::service_traits::DataType<RosService>::value();
  }

  virtual std::string GetServiceMD5Sum() const {
    return ros::service_traits::MD5Sum<RosService>::value();
  }

  const Request* request_;
  Response* response_;
  RosServiceRequest service_request_;
  std::unique_ptr<Channel> channel_;
  RosRpcHeader header_;
  MessageReceiver<Response> receiver_;
  StatusOnceCallback connect_callback_;
  StatusOnceCallback call_callback_;
};

template <typename T>
void FEL_ROS_CLIENT::Connect(const IPEndPoint& ip_endpoint,
                             StatusOnceCallback callback) {
  DCHECK(connect_callback_.is_null());
  channel_ = ChannelFactory::NewChannel(ChannelDef::CHANNEL_TYPE_TCP);
  TCPChannel* tcp_channel = channel_->ToTCPChannel();
  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::CHANNEL_TYPE_TCP);
  *channel_def.mutable_ip_endpoint() = ip_endpoint;
  connect_callback_ = std::move(callback);
  tcp_channel->Connect(channel_def, base::BindOnce(&FEL_ROS_CLIENT::OnConnect,
                                                   base::Unretained(this)));
}

template <typename T>
void FEL_ROS_CLIENT::Call(const Request* request, Response* response,
                          StatusOnceCallback callback) {
  if (!call_callback_.is_null()) {
    std::move(callback).Run(
        errors::Aborted("You already called and it is waiting for response."));
  } else if (!channel_) {
    std::move(callback).Run(errors::Aborted("channel is not connected."));
  } else {
    request_ = request;
    response_ = response;
    call_callback_ = std::move(callback);
    MaybeSendRequest();
  }
}

template <typename T>
void FEL_ROS_CLIENT::OnConnect(const Status& s) {
  if (!s.ok()) {
    LOG(ERROR) << s;
    Shutdown();
    return;
  }

  if (IsUsingRosProtocol(service_info_.service())) {
    service_request_.Request(
        this, base::BindOnce(&FEL_ROS_CLIENT::OnRosServiceHandshake,
                             base::Unretained(this)));
  } else {
    channel_->SetDynamicSendBuffer(true);
    channel_->SetDynamicReceiveBuffer(true);
    receiver_.set_channel(channel_.get());
    std::move(connect_callback_).Run(s);
  }
}

template <typename T>
void FEL_ROS_CLIENT::OnRosServiceHandshake(const Status& s) {
  if (!s.ok()) {
    LOG(ERROR) << s;
    Shutdown();
    return;
  }

  channel_->SetDynamicSendBuffer(true);
  channel_->SetDynamicReceiveBuffer(true);
  receiver_.set_channel(channel_.get());
  receiver_.set_header_size_callback(base::BindRepeating(
      &RosRpcHeader::header_size, base::Unretained(&header_)));
  receiver_.set_parse_header_callback(base::BindRepeating(
      &RosRpcHeader::ParseHeader, base::Unretained(&header_)));
  std::move(connect_callback_).Run(s);
}

template <typename T>
Status FEL_ROS_CLIENT::Run() {
  MaybeSendRequest();
  return Status::OK();
}

template <typename T>
void FEL_ROS_CLIENT::MaybeSendRequest() {
  DCHECK(channel_);
  if (channel_->IsConnected() && !call_callback_.is_null()) {
    SendRequest();
  }
}

template <typename T>
void FEL_ROS_CLIENT::SendRequest() {
  MessageSender<Request> sender(channel_.get());
  sender.SendMessage(*request_, base::BindOnce(&FEL_ROS_CLIENT::OnSendRequest,
                                               base::Unretained(this)));
}

template <typename T>
void FEL_ROS_CLIENT::OnSendRequest(const Status& s) {
  DCHECK(!call_callback_.is_null());
  if (s.ok()) {
    ReceiveResponse();
  } else {
    std::move(call_callback_).Run(s);
  }
}

template <typename T>
void FEL_ROS_CLIENT::ReceiveResponse() {
  receiver_.ReceiveMessage(base::BindOnce(&FEL_ROS_CLIENT::OnReceiveResponse,
                                          base::Unretained(this)));
}

template <typename T>
void FEL_ROS_CLIENT::OnReceiveResponse(const Status& s) {
  DCHECK(!call_callback_.is_null());
  if (s.ok() && header_.ok()) {
    *response_ = std::move(std::move(receiver_).message());
    std::move(call_callback_).Run(Status::OK());
  } else {
    if (!s.ok()) {
      std::move(call_callback_).Run(s);
    } else {
      std::move(call_callback_).Run(errors::Unknown("Failed to rpc call."));
    }
  }
}

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_CLIENT_IMPL_H_

#endif  // defined(HAS_ROS)