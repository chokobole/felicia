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

  ChannelDef::Type type() const override { return ChannelDef::UDP; }

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
  bool ret = ToNetIPEndPoint(channel_def, &ip_endpoint);
  DCHECK(ret);
  this->channel_impl_ = std::make_unique<UDPClientSocket>();
  UDPClientSocket* client_socket =
      this->channel_impl_->ToSocket()->ToUDPSocket()->ToUDPClientSocket();
  client_socket->Connect(ip_endpoint, std::move(callback));
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_