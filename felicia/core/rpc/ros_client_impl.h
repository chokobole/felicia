#if defined(HAS_ROS)

#ifndef FELICIA_CORE_RPC_ROS_CLIENT_IMPL_H_
#define FELICIA_CORE_RPC_ROS_CLIENT_IMPL_H_

#include <ros/service_traits.h>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/channel/ros_header.h"
#include "felicia/core/channel/ros_protocol.h"
#include "felicia/core/lib/error/errors.h"
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
    return Status::OK();
  }

  void Call(const Request* request, Response* response,
            StatusOnceCallback callback);

 protected:
  void OnConnect(const Status& s);
  void OnWriteRosHeader(ChannelDef::Type, const Status& s);
  void OnReadRosHeader(std::string* buffer, const Status& s);
  // SendRequest if it is connected.
  void MaybeSendRequest();
  void SendRequest();
  void OnSendRequest(ChannelDef::Type, const Status& s);
  void ReceiveResponse();
  void OnReceiveResponse(const std::string* receive_buffer, const Status& s);

  virtual std::string GetServiceDataType() const {
    return ros::service_traits::DataType<RosService>::value();
  }

  virtual std::string GetServiceMD5Sum() const {
    return ros::service_traits::MD5Sum<RosService>::value();
  }

  const Request* request_;
  Response* response_;
  std::unique_ptr<Channel<Request>> channel_;
  StatusOnceCallback connect_callback_;
  StatusOnceCallback call_callback_;
};

template <typename T>
void FEL_ROS_CLIENT::Connect(const IPEndPoint& ip_endpoint,
                             StatusOnceCallback callback) {
  DCHECK(connect_callback_.is_null());
  channel_ = ChannelFactory::NewChannel<Request>(ChannelDef::CHANNEL_TYPE_TCP);
  channel_->SetDynamicSendBuffer(true);
  channel_->SetDynamicReceiveBuffer(true);
  channel_->set_use_ros_channel(IsUsingRosProtocol(service_info_.service()));
  TCPChannel<Request>* tcp_channel = channel_->ToTCPChannel();
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
  }
  if (channel_->use_ros_channel()) {
    RosServiceRequestHeader header;
    ConsumeRosProtocol(service_info_.service(), &header.service);
    header.md5sum = GetServiceMD5Sum();
    header.callerid = service_info_.ros_node_name();
    header.persistent = "1";
    std::string send_buffer;
    header.WriteToBuffer(&send_buffer);

    channel_->SetDynamicSendBuffer(true);
    channel_->SendRawMessage(
        send_buffer, false,
        base::BindRepeating(&FEL_ROS_CLIENT::OnWriteRosHeader,
                            base::Unretained(this)));

    channel_->SetDynamicReceiveBuffer(true);
    std::string* receive_buffer = new std::string();
    channel_->ReceiveRawMessage(
        receive_buffer,
        base::BindOnce(&FEL_ROS_CLIENT::OnReadRosHeader, base::Unretained(this),
                       base::Owned(receive_buffer)));
  } else {
    std::move(connect_callback_).Run(s);
  }
}

template <typename T>
void FEL_ROS_CLIENT::OnWriteRosHeader(ChannelDef::Type, const Status& s) {
  LOG_IF(ERROR, !s.ok()) << s;
  channel_->SetDynamicSendBuffer(false);
}

template <typename T>
void FEL_ROS_CLIENT::OnReadRosHeader(std::string* buffer, const Status& s) {
  RosServiceResponseHeader header;
  Status new_status = s;
  if (new_status.ok()) {
    new_status = header.ReadFromBuffer(*buffer);
    if (new_status.ok()) {
      RosServiceResponseHeader expected;
      expected.md5sum = GetServiceMD5Sum();
      new_status = header.Validate(expected);
    }
  }

  if (!new_status.ok()) {
    channel_.reset();
  } else {
    channel_->SetDynamicReceiveBuffer(false);
  }
  std::move(connect_callback_).Run(new_status);
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
  channel_->SendMessage(*request_,
                        base::BindRepeating(&FEL_ROS_CLIENT::OnSendRequest,
                                            base::Unretained(this)));
}

template <typename T>
void FEL_ROS_CLIENT::OnSendRequest(ChannelDef::Type, const Status& s) {
  DCHECK(!call_callback_.is_null());
  if (s.ok()) {
    ReceiveResponse();
  } else {
    std::move(call_callback_).Run(s);
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
  DCHECK(!call_callback_.is_null());
  if (s.ok()) {
    MessageIOError err = MessageIOImpl<Response>::Deserialize(
        receive_buffer->c_str(), receive_buffer->length(), response_);
    if (err == MessageIOError::OK) {
      std::move(call_callback_).Run(Status::OK());
    } else {
      std::move(call_callback_)
          .Run(errors::Unknown(MessageIOErrorToString(err)));
    }
  } else {
    std::move(call_callback_).Run(s);
    Shutdown();
  }
}

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_CORE_RPC_ROS_CLIENT_IMPL_H_

#endif  // defined(HAS_ROS)