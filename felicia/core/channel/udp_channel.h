#ifndef FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_

#include <memory>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/ip_endpoint.h"
#include "third_party/chromium/net/socket/udp_socket.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/udp_client_channel.h"
#include "felicia/core/channel/udp_server_channel.h"
#include "felicia/core/lib/error/status.h"

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

  StatusOr<ChannelSource> Bind();

  void Connect(const ChannelSource& channel_source,
               StatusOnceCallback callback) override;

  void SetSendBufferSize(Bytes bytes) override {
    if (bytes > kMaximumBufferSize) {
      LOG(ERROR) << "UDP buffer can't exceed " << bytes << "Bytes";
      bytes = kMaximumBufferSize;
    }
    this->send_buffer_.resize(bytes.bytes());
  }
  void SetReceiveBufferSize(Bytes bytes) override {
    if (bytes > kMaximumBufferSize) {
      LOG(ERROR) << "UDP buffer can't exceed " << bytes << "Bytes";
      bytes = kMaximumBufferSize;
    }
    this->receive_buffer_.resize(bytes.bytes());
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(UDPChannel);
};

template <typename MessageTy>
UDPChannel<MessageTy>::UDPChannel() {}

template <typename MessageTy>
UDPChannel<MessageTy>::~UDPChannel() {}

template <typename MessageTy>
StatusOr<ChannelSource> UDPChannel<MessageTy>::Bind() {
  DCHECK(!this->channel_);
  this->channel_ = std::make_unique<UDPServerChannel>();
  return this->channel_->ToUDPChannelBase()->ToUDPServerChannel()->Bind();
}

template <typename MessageTy>
void UDPChannel<MessageTy>::Connect(const ChannelSource& channel_source,
                                    StatusOnceCallback callback) {
  DCHECK(!this->channel_);
  DCHECK(!callback.is_null());
  ::net::IPEndPoint ip_endpoint;
  bool ret = ToNetIPEndPoint(channel_source, &ip_endpoint);
  DCHECK(ret);
  this->channel_ = std::make_unique<UDPClientChannel>();
  this->channel_->ToUDPChannelBase()->ToUDPClientChannel()->Connect(
      ip_endpoint, std::move(callback));
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_UDP_CHANNEL_H_