#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_CLIENT_IMPL_H_
#define FELICIA_CORE_RPC_ROS_CLIENT_IMPL_H_

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/rpc/ros_util.h"

namespace felicia {
namespace rpc {

#define FEL_ROS_CLIENT Client<T, std::enable_if_t<IsRosService<T>::value>>

template <typename T>
class FEL_ROS_CLIENT {
 public:
  typedef T RosService;
  typedef typename RosService::Request Request;
  typedef typename RosService::Response Response;

  Client() = default;
  ~Client() = default;

  Status ConnectAndRun(const IPEndPoint& ip_endpoint);

  Status Shutdown() {
    channel_.reset();
    return Status::OK();
  }

  void Call(const Request* request, Response* response,
            StatusOnceCallback callback);

 private:
  void OnConnect(const Status& s);
  // SendRequest if it is connected.
  void MaybeSendRequest();
  void SendRequest();
  void OnSendRequest(ChannelDef::Type, const Status& s);
  void ReceiveResponse();
  void OnReceiveResponse(const std::string* receive_buffer, const Status& s);

  const Request* request_;
  Response* response_;
  std::unique_ptr<Channel<Request>> channel_;
  StatusOnceCallback callback_;
};

template <typename T>
Status FEL_ROS_CLIENT::ConnectAndRun(const IPEndPoint& ip_endpoint) {
  channel_ = ChannelFactory::NewChannel<Request>(ChannelDef::CHANNEL_TYPE_TCP);
  channel_->EnableDynamicBuffer();
  TCPChannel<Request>* tcp_channel = channel_->ToTCPChannel();
  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::CHANNEL_TYPE_TCP);
  *channel_def.mutable_ip_endpoint() = ip_endpoint;
  tcp_channel->Connect(channel_def, base::BindOnce(&FEL_ROS_CLIENT::OnConnect,
                                                   base::Unretained(this)));
  return Status::OK();
}

template <typename T>
void FEL_ROS_CLIENT::Call(const Request* request, Response* response,
                          StatusOnceCallback callback) {
  if (!callback_.is_null()) {
    std::move(callback).Run(
        errors::Aborted("You already called and it is waiting for response."));
  } else if (!channel_) {
    std::move(callback).Run(errors::Aborted("channel is not connected."));
  } else {
    request_ = request;
    response_ = response;
    callback_ = std::move(callback);
    MaybeSendRequest();
  }
}

template <typename T>
void FEL_ROS_CLIENT::OnConnect(const Status& s) {
  if (s.ok()) {
    MaybeSendRequest();
  } else {
    LOG(ERROR) << s;
    Shutdown();
  }
}

template <typename T>
void FEL_ROS_CLIENT::MaybeSendRequest() {
  DCHECK(channel_);
  if (channel_->IsConnected() && !callback_.is_null()) {
    SendRequest();
  }
}

template <typename T>
void FEL_ROS_CLIENT::SendRequest() {
  channel_->SendMessage(*request_,
                        base::BindRepeating(&FEL_ROS_CLIENT::OnSendRequest,
                                            base::Unretained(this)));
}

template <typename T>
void FEL_ROS_CLIENT::OnSendRequest(ChannelDef::Type, const Status& s) {
  DCHECK(!callback_.is_null());
  if (s.ok()) {
    ReceiveResponse();
  } else {
    std::move(callback_).Run(s);
    Shutdown();
  }
}

template <typename T>
void FEL_ROS_CLIENT::ReceiveResponse() {
  std::string* receive_buffer = new std::string();
  channel_->ReceiveRawMessage(
      receive_buffer,
      base::BindOnce(&FEL_ROS_CLIENT::OnReceiveResponse, base::Unretained(this),
                     base::Owned(receive_buffer)));
}

template <typename T>
void FEL_ROS_CLIENT::OnReceiveResponse(const std::string* receive_buffer,
                                       const Status& s) {
  DCHECK(!callback_.is_null());
  if (s.ok()) {
    MessageIOError err = MessageIOImpl<Response>::Deserialize(
        receive_buffer->c_str(), receive_buffer->length(), response_);
    if (err == MessageIOError::OK) {
      std::move(callback_).Run(Status::OK());
    } else {
      std::move(callback_).Run(errors::Unknown(MessageIOErrorToString(err)));
    }
  } else {
    std::move(callback_).Run(s);
    Shutdown();
  }
}

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_CLIENT_IMPL_H_

#endif  // defined(HAS_ROS)