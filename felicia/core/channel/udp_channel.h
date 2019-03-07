#ifndef FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_

#include <memory>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/strings/strcat.h"
#include "third_party/chromium/net/base/ip_endpoint.h"
#include "third_party/chromium/net/base/net_errors.h"
#include "third_party/chromium/net/socket/udp_socket.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/udp_client_channel.h"
#include "felicia/core/channel/udp_server_channel.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/message/message.h"

namespace felicia {

template <typename MessageTy>
class UDPChannel : public Channel<MessageTy> {
 public:
  UDPChannel();
  ~UDPChannel();

  bool IsUDPChannel() const override { return true; }

  StatusOr<ChannelSource> Bind();

  void Connect(const ChannelSource& channel_source,
               StatusCallback callback) override;

  void SendMessage(const MessageTy& message, StatusCallback callback) override;

  void ReceiveMessage(MessageTy* message, StatusCallback callback) override;

 private:
  void OnSendMessage(scoped_refptr<::net::IOBufferWithSize> buffer,
                     const Status& s);
  void OnReceiveMessage(scoped_refptr<::net::IOBufferWithSize> buffer,
                        const Status& s);

  std::unique_ptr<UDPChannelBase> udp_channel_;

  DISALLOW_COPY_AND_ASSIGN(UDPChannel);
};

template <typename MessageTy>
UDPChannel<MessageTy>::UDPChannel() {}

template <typename MessageTy>
UDPChannel<MessageTy>::~UDPChannel() {}

template <typename MessageTy>
StatusOr<ChannelSource> UDPChannel<MessageTy>::Bind() {
  DCHECK(!udp_channel_);
  udp_channel_ = std::make_unique<UDPServerChannel>();
  return udp_channel_->ToUDPServerChannel()->Bind();
}

template <typename MessageTy>
void UDPChannel<MessageTy>::Connect(const ChannelSource& channel_source,
                                    StatusCallback callback) {
  DCHECK(!udp_channel_);
  DCHECK(!callback.is_null());
  ::net::IPEndPoint ip_endpoint;
  bool ret = ToNetIPEndPoint(channel_source, &ip_endpoint);
  DCHECK(ret);
  udp_channel_ = std::make_unique<UDPClientChannel>();
  udp_channel_->ToUDPClientChannel()->Connect(ip_endpoint, std::move(callback));
}

template <typename MessageTy>
void UDPChannel<MessageTy>::SendMessage(const MessageTy& message,
                                        StatusCallback callback) {
  DCHECK(udp_channel_);
  DCHECK(this->send_callback_.is_null());
  DCHECK(!callback.is_null());
  scoped_refptr<::net::IOBufferWithSize> buffer;
  if (Message<MessageTy>::SerializeToBuffer(&message, &buffer)) {
    this->send_callback_ = std::move(callback);
    DLOG(INFO) << "SendMessage() write bytes: " << buffer->size();
    udp_channel_->Write(buffer.get(),
                        ::base::BindOnce(&UDPChannel<MessageTy>::OnSendMessage,
                                         ::base::Unretained(this), buffer));
  } else {
    std::move(callback).Run(errors::InvalidArgument(::base::StrCat(
        {"message is invalid, which is", message.DebugString().c_str()})));
  }
}

template <typename MessageTy>
void UDPChannel<MessageTy>::OnSendMessage(
    scoped_refptr<::net::IOBufferWithSize> buffer, const Status& s) {
  // |buffer| is copied for the reference count.
  std::move(this->send_callback_).Run(s);
}

template <typename MessageTy>
void UDPChannel<MessageTy>::ReceiveMessage(MessageTy* message,
                                           StatusCallback callback) {
  DCHECK(udp_channel_);
  DCHECK(this->receive_callback_.is_null());
  DCHECK(!callback.is_null());
  scoped_refptr<::net::IOBufferWithSize> buffer =
      ::base::MakeRefCounted<::net::IOBufferWithSize>(
          ChannelBase::kMaxReceiverBufferSize);

  this->message_ = message;
  this->receive_callback_ = std::move(callback);
  udp_channel_->Read(buffer.get(),
                     ::base::BindOnce(&UDPChannel<MessageTy>::OnReceiveMessage,
                                      ::base::Unretained(this), buffer));
}

template <typename MessageTy>
void UDPChannel<MessageTy>::OnReceiveMessage(
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

#endif  // FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_