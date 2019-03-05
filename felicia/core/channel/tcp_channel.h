#ifndef FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_

#include <memory>
#include <utility>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/net/base/ip_endpoint.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/tcp_client_channel.h"
#include "felicia/core/channel/tcp_server_channel.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/message/message.h"

namespace felicia {

template <typename MessageTy>
class TCPChannel : public Channel<MessageTy> {
 public:
  TCPChannel();
  ~TCPChannel();

  bool IsTCPChannel() const override { return true; }

  bool IsConnected() const {
    DCHECK(tcp_channel_);
    return tcp_channel_->IsConnected();
  }

  void Listen(StatusOrChannelSourceCallback listen_callback,
              TCPServerChannel::AcceptCallback accept_callback);

  void Connect(const ChannelSource& channel_source,
               StatusCallback callback) override;

  void SendMessage(const MessageTy& message, StatusCallback callback) override;

  void ReceiveMessage(MessageTy* message, StatusCallback callback) override;

 private:
  void OnSendMessage(scoped_refptr<::net::IOBufferWithSize> buffer,
                     const Status& s);
  void OnReceiveMessage(scoped_refptr<::net::IOBufferWithSize> buffer,
                        const Status& s);

  std::unique_ptr<TCPChannelBase> tcp_channel_;

  DISALLOW_COPY_AND_ASSIGN(TCPChannel);
};

template <typename MessageTy>
TCPChannel<MessageTy>::TCPChannel() {}

template <typename MessageTy>
TCPChannel<MessageTy>::~TCPChannel() = default;

template <typename MessageTy>
void TCPChannel<MessageTy>::Listen(
    StatusOrChannelSourceCallback listen_callback,
    TCPServerChannel::AcceptCallback accept_callback) {
  DCHECK(!tcp_channel_);
  DCHECK(!listen_callback.is_null());
  DCHECK(!accept_callback.is_null());
  tcp_channel_ = std::make_unique<TCPServerChannel>();
  TCPServerChannel* server_channel = tcp_channel_->ToTCPServerChannel();
  server_channel->set_accept_callback(accept_callback);
  server_channel->Listen(std::move(listen_callback));
}

template <typename MessageTy>
void TCPChannel<MessageTy>::Connect(const ChannelSource& channel_source,
                                    StatusCallback callback) {
  DCHECK(!tcp_channel_);
  DCHECK(!callback.is_null());
  ::net::IPEndPoint ip_endpoint;
  bool ret = ToNetIPEndPoint(channel_source, &ip_endpoint);
  DCHECK(ret);
  tcp_channel_ = std::make_unique<TCPClientChannel>();
  tcp_channel_->ToTCPClientChannel()->Connect(ip_endpoint, std::move(callback));
}

template <typename MessageTy>
void TCPChannel<MessageTy>::SendMessage(const MessageTy& message,
                                        StatusCallback callback) {
  DCHECK(tcp_channel_);
  DCHECK(this->send_callback_.is_null());
  DCHECK(!callback.is_null());
  scoped_refptr<::net::IOBufferWithSize> buffer;
  if (Message<MessageTy>::SerializeToBuffer(&message, &buffer)) {
    this->send_callback_ = std::move(callback);
    DLOG(INFO) << "SendMessage() write bytes: " << buffer->size();
    tcp_channel_->Write(buffer.get(),
                        ::base::BindOnce(&TCPChannel<MessageTy>::OnSendMessage,
                                         ::base::Unretained(this), buffer));
  } else {
    std::move(callback).Run(errors::InvalidArgument(::base::StrCat(
        {"message is invalid, which is", message.DebugString().c_str()})));
  }
}

template <typename MessageTy>
void TCPChannel<MessageTy>::OnSendMessage(
    scoped_refptr<::net::IOBufferWithSize> buffer, const Status& s) {
  // |buffer| is copied for the reference count.
  std::move(this->send_callback_).Run(s);
}

template <typename MessageTy>
void TCPChannel<MessageTy>::ReceiveMessage(MessageTy* message,
                                           StatusCallback callback) {
  DCHECK(tcp_channel_);
  DCHECK(this->receive_callback_.is_null());
  DCHECK(!callback.is_null());
  scoped_refptr<::net::IOBufferWithSize> buffer =
      ::base::MakeRefCounted<::net::IOBufferWithSize>(
          ChannelBase::kMaxReceiverBufferSize);

  this->message_ = message;
  this->receive_callback_ = std::move(callback);
  tcp_channel_->Read(buffer.get(),
                     ::base::BindOnce(&TCPChannel<MessageTy>::OnReceiveMessage,
                                      ::base::Unretained(this), buffer));
}

template <typename MessageTy>
void TCPChannel<MessageTy>::OnReceiveMessage(
    scoped_refptr<::net::IOBufferWithSize> buffer, const Status& s) {
  if (s.ok()) {
    bool ret = Message<MessageTy>::ParseFromBuffer(buffer, this->message_);
    if (!ret) {
      std::move(this->receive_callback_)
          .Run(errors::DataLoss("Failed to parse from buffer"));
      return;
    }
  }

  std::move(this->receive_callback_).Run(s);
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_