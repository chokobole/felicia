#ifndef FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/socket/udp_client_socket.h"
#include "felicia/core/channel/socket/udp_server_socket.h"

namespace felicia {

namespace {
Bytes kMaximumBufferSize = Bytes::FromKilloBytes(64);
}  // namespace

template <typename MessageTy>
class UDPChannel : public Channel<MessageTy> {
 public:
  UDPChannel();
  ~UDPChannel();

  bool IsUDPChannel() const override { return true; }

  ChannelDef::Type type() const override {
    return ChannelDef::CHANNEL_TYPE_UDP;
  }

  StatusOr<ChannelDef> Bind();

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

  void SetSendBufferSize(Bytes bytes) override {
    if (bytes > kMaximumBufferSize) {
      LOG(ERROR) << "UDP buffer can't exceed " << bytes;
      bytes = kMaximumBufferSize;
    }
    this->send_buffer_->SetCapacity(bytes.bytes());
  }
  void SetReceiveBufferSize(Bytes bytes) override {
    if (bytes > kMaximumBufferSize) {
      LOG(ERROR) << "UDP buffer can't exceed " << bytes;
      bytes = kMaximumBufferSize;
    }
    this->receive_buffer_->SetCapacity(bytes.bytes());
  }

 private:
  void ReadImpl(MessageTy* message, StatusOnceCallback callback) override;
  void OnReceiveMessageWithHeader(const Status& s);

  DISALLOW_COPY_AND_ASSIGN(UDPChannel);
};

template <typename MessageTy>
UDPChannel<MessageTy>::UDPChannel() {}

template <typename MessageTy>
UDPChannel<MessageTy>::~UDPChannel() {}

template <typename MessageTy>
StatusOr<ChannelDef> UDPChannel<MessageTy>::Bind() {
  DCHECK(!this->channel_impl_);
  this->channel_impl_ = std::make_unique<UDPServerSocket>();
  UDPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToUDPSocket()->ToUDPServerSocket();
  return server_socket->Bind();
}

template <typename MessageTy>
void UDPChannel<MessageTy>::Connect(const ChannelDef& channel_def,
                                    StatusOnceCallback callback) {
  DCHECK(!this->channel_impl_);
  DCHECK(!callback.is_null());
  ::net::IPEndPoint ip_endpoint;
  Status s = ToNetIPEndPoint(channel_def, &ip_endpoint);
  if (!s.ok()) {
    std::move(callback).Run(s);
    return;
  }
  this->channel_impl_ = std::make_unique<UDPClientSocket>();
  UDPClientSocket* client_socket =
      this->channel_impl_->ToSocket()->ToUDPSocket()->ToUDPClientSocket();
  client_socket->Connect(ip_endpoint, std::move(callback));
}

template <typename MessageTy>
void UDPChannel<MessageTy>::ReadImpl(MessageTy* message,
                                     StatusOnceCallback callback) {
  if (this->is_dynamic_buffer_ && this->receive_buffer_->capacity() == 0) {
    SetReceiveBufferSize(kMaximumBufferSize);
  }

  this->message_ = message;
  this->receive_callback_ = std::move(callback);
  this->channel_impl_->Read(
      this->receive_buffer_, this->receive_buffer_->capacity(),
      ::base::BindOnce(&UDPChannel<MessageTy>::OnReceiveMessageWithHeader,
                       ::base::Unretained(this)));
}

template <typename MessageTy>
void UDPChannel<MessageTy>::OnReceiveMessageWithHeader(const Status& s) {
  if (!s.ok()) {
    std::move(this->receive_callback_).Run(s);
    return;
  }

  MessageIoError err = MessageIO<MessageTy>::ParseHeaderFromBuffer(
      this->receive_buffer_->StartOfBuffer(), &this->header_);
  if (err != MessageIoError::OK) {
    std::move(this->receive_callback_)
        .Run(errors::DataLoss(MessageIoErrorToString(err)));
    return;
  }

  if (this->receive_buffer_->capacity() - sizeof(Header) <
      this->header_.size()) {
    std::move(this->receive_callback_)
        .Run(errors::Aborted(
            MessageIoErrorToString(MessageIoError::ERR_NOT_ENOUGH_BUFFER)));
    return;
  }

  err = MessageIO<MessageTy>::ParseMessageFromBuffer(
      this->receive_buffer_->StartOfBuffer(), this->header_, true,
      this->message_);
  if (err != MessageIoError::OK) {
    std::move(this->receive_callback_)
        .Run(errors::DataLoss("Failed to parse message from buffer."));
    return;
  }

  std::move(this->receive_callback_).Run(s);
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_