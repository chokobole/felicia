#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_SERVER_IMPL_H_
#define FELICIA_CORE_RPC_ROS_SERVER_IMPL_H_

#include "third_party/chromium/base/memory/scoped_refptr.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/message/message_io.h"
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
                    std::unique_ptr<TCPChannel<Response>> channel)
      : service_(service), channel_(std::move(channel)) {
    channel_->SetDynamicSendBuffer(true);
    channel_->SetDynamicReceiveBuffer(true);
    ReceiveRequest();
  }

 private:
  void ReceiveRequest();
  void OnReceiveRequest(const std::string* receive_buffer, const Status& s);
  void OnHandleRequest(const Status& s);
  void OnSendResponse(ChannelDef::Type, const Status& s);

  Request request_;
  Response response_;
  scoped_refptr<Service> service_;
  std::unique_ptr<TCPChannel<Response>> channel_;
};

template <typename Service, typename Request, typename Response>
void RosServiceHandler<Service, Request, Response>::ReceiveRequest() {
  std::string* receive_buffer = new std::string();
  channel_->ReceiveRawMessage(
      receive_buffer,
      base::BindOnce(
          &RosServiceHandler<Service, Request, Response>::OnReceiveRequest,
          base::Unretained(this), base::Owned(receive_buffer)));
}

template <typename Service, typename Request, typename Response>
void RosServiceHandler<Service, Request, Response>::OnReceiveRequest(
    const std::string* receive_buffer, const Status& s) {
  if (s.ok()) {
    MessageIOError err = MessageIOImpl<Request>::Deserialize(
        receive_buffer->c_str(), receive_buffer->length(), &request_);
    if (err == MessageIOError::OK) {
      service_->Handle(
          &request_, &response_,
          base::BindOnce(
              &RosServiceHandler<Service, Request, Response>::OnHandleRequest,
              base::Unretained(this)));
    }
  } else {
    delete this;
  }
}

template <typename Service, typename Request, typename Response>
void RosServiceHandler<Service, Request, Response>::OnHandleRequest(
    const Status& s) {
  if (s.ok()) {
    channel_->SendMessage(
        response_,
        base::BindRepeating(
            &RosServiceHandler<Service, Request, Response>::OnSendResponse,
            base::Unretained(this)));
  } else {
    LOG(ERROR) << s;
    ReceiveRequest();
  }
}

template <typename Service, typename Request, typename Response>
void RosServiceHandler<Service, Request, Response>::OnSendResponse(
    ChannelDef::Type, const Status& s) {
  if (s.ok()) {
    ReceiveRequest();
  } else {
    delete this;
  }
}

}  // namespace internal

#define FEL_ROS_SERVER \
  Server<T, std::enable_if_t<IsRosService<typename T::RosService>::value>>

template <typename T>
class FEL_ROS_SERVER : public ServerInterface {
 public:
  typedef T Service;
  typedef typename Service::Request Request;
  typedef typename Service::Response Response;

  Server() = default;
  ~Server() override = default;

  Status Start() override {
    service_ = base::MakeRefCounted<Service>();
    return Status::OK();
  }

  // Non-blocking
  Status Run() override;

  Status Shutdown() override {
    channel_.reset();
    return Status::OK();
  }

  std::string service_name() const override { return Service::DataType(); }

 private:
  void DoAcceptLoop();
  void OnAccept(StatusOr<std::unique_ptr<TCPChannel<Response>>> status_or);

  scoped_refptr<Service> service_;
  std::unique_ptr<Channel<Response>> channel_;
};

template <typename T>
Status Server<
    T, std::enable_if_t<IsRosService<typename T::RosService>::value>>::Run() {
  channel_ = ChannelFactory::NewChannel<Response>(ChannelDef::CHANNEL_TYPE_TCP);
  TCPChannel<Response>* tcp_channel = channel_->ToTCPChannel();
  auto status_or = tcp_channel->Listen();
  if (status_or.ok()) {
    port_ = status_or.ValueOrDie().ip_endpoint().port();
    DoAcceptLoop();
    return Status::OK();
  }
  return status_or.status();
}

template <typename T>
void FEL_ROS_SERVER::DoAcceptLoop() {
  TCPChannel<Response>* tcp_channel = channel_->ToTCPChannel();
  tcp_channel->AcceptOnceIntercept(base::BindRepeating(
      &Server<T, std::enable_if_t<
                     IsRosService<typename T::RosService>::value>>::OnAccept,
      base::Unretained(this)));
}

template <typename T>
void FEL_ROS_SERVER::OnAccept(
    StatusOr<std::unique_ptr<TCPChannel<Response>>> status_or) {
  if (status_or.ok()) {
    new internal::RosServiceHandler<Service, Request, Response>(
        service_, std::move(status_or.ValueOrDie()));
  }
  DoAcceptLoop();
}

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_SERVER_IMPL_H_

#endif  // defined(HAS_ROS)