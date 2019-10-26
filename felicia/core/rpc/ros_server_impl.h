#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_SERVER_IMPL_H_
#define FELICIA_CORE_RPC_ROS_SERVER_IMPL_H_

#include "third_party/chromium/base/memory/scoped_refptr.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/channel/message_receiver.h"
#include "felicia/core/channel/message_sender.h"
#include "felicia/core/channel/ros_service_response.h"
#include "felicia/core/message/message_io.h"
#include "felicia/core/message/ros_protocol.h"
#include "felicia/core/message/ros_rpc_header.h"
#include "felicia/core/rpc/ros_serialized_service_interface.h"
#include "felicia/core/rpc/ros_util.h"
#include "felicia/core/rpc/server_interface.h"

namespace felicia {
namespace rpc {

namespace internal {

// TODO: has to delete itself, when hasn't been used so long.
template <typename Service, typename Request, typename Response>
class RosServiceHandler {
 public:
  RosServiceHandler(scoped_refptr<Service> service,
                    std::unique_ptr<TCPChannel> channel, bool use_ros_protocol)
      : service_(service),
        channel_(std::move(channel)),
        use_ros_protocol_(use_ros_protocol) {
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
  scoped_refptr<Service> service_;
  std::unique_ptr<TCPChannel> channel_;
  MessageReceiver<Request> receiver_;
  bool use_ros_protocol_;
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
    service_->Handle(
        &receiver_.message(), &response_,
        base::BindOnce(
            &RosServiceHandler<Service, Request, Response>::OnHandleRequest,
            base::Unretained(this)));
  } else {
    LOG(ERROR) << s;
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
    ReceiveRequest();
  } else {
    LOG(ERROR) << s;
  }
}

template <typename Service, typename Request, typename Response>
void RosServiceHandler<Service, Request, Response>::OnHandleRequest(Status s) {
  LOG_IF(ERROR, !s.ok()) << s;
  SendResponse(s.ok());
}

}  // namespace internal

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

  Server() : service_(base::MakeRefCounted<Service>()) {}
  explicit Server(scoped_refptr<Service> service)
      : service_(std::move(service)) {}
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
    service_handlers_.clear();
    return Status::OK();
  }

  std::string GetServiceTypeName() const override {
    return service_->GetServiceTypeName();
  }

  std::string GetServiceMD5Sum() const { return service_->GetServiceMD5Sum(); }

  std::string GetRequestTypeName() const {
    return service_->GetRequestTypeName();
  }

  std::string GetResponseTypeName() const {
    return service_->GetResponseTypeName();
  }

 protected:
  friend class felicia::RosServiceResponse;

  void DoAcceptLoop();
  void OnAccept(StatusOr<std::unique_ptr<TCPChannel>> status_or);
  void OnRosServiceHandshake(std::unique_ptr<Channel> client_channel);

  scoped_refptr<Service> service_;
  std::unique_ptr<Channel> channel_;
  std::vector<std::unique_ptr<ServiceHandler>> service_handlers_;
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
      base::BindRepeating(&FEL_ROS_SERVER::OnAccept, base::Unretained(this)));
}

template <typename T>
void FEL_ROS_SERVER::OnAccept(StatusOr<std::unique_ptr<TCPChannel>> status_or) {
  if (status_or.ok()) {
    if (IsUsingRosProtocol(service_info_.service())) {
      // It deletes itself when handshake is completed.
      RosServiceResponse* service_response =
          new RosServiceResponse(std::move(status_or).ValueOrDie());
      service_response->ReceiveRequest(
          this, base::BindOnce(&FEL_ROS_SERVER::OnRosServiceHandshake,
                               base::Unretained(this)));
    } else {
      service_handlers_.push_back(std::make_unique<ServiceHandler>(
          service_, std::move(status_or).ValueOrDie(), false));
    }
  }
  DoAcceptLoop();
}

template <typename T>
void FEL_ROS_SERVER::OnRosServiceHandshake(
    std::unique_ptr<Channel> client_channel) {
  std::unique_ptr<TCPChannel> client_tcp_channel;
  client_tcp_channel.reset(
      reinterpret_cast<TCPChannel*>(client_channel.release()));
  service_handlers_.push_back(std::make_unique<ServiceHandler>(
      service_, std::move(client_tcp_channel), true));
}

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_SERVER_IMPL_H_

#endif  // defined(HAS_ROS)