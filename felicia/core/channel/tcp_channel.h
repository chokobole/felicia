#ifndef FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_

#include <memory>
#include <utility>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/tcp_client_channel.h"
#include "felicia/core/channel/tcp_server_channel.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

template <typename MessageTy>
class TCPChannel : public Channel<MessageTy> {
 public:
  TCPChannel();
  ~TCPChannel();

  bool IsTCPChannel() const override { return true; }

  bool IsConnected() const {
    DCHECK(this->channel_);
    return this->channel_->ToTCPChannelBase()->IsConnected();
  }

  StatusOr<ChannelSource> Listen();

  void DoAcceptLoop(TCPServerChannel::AcceptCallback accept_callback);

  void AcceptOnce(TCPServerChannel::AcceptOnceCallback accept_once_callback);

  void Connect(const ChannelSource& channel_source,
               StatusCallback callback) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(TCPChannel);
};

template <typename MessageTy>
TCPChannel<MessageTy>::TCPChannel() {}

template <typename MessageTy>
TCPChannel<MessageTy>::~TCPChannel() = default;

template <typename MessageTy>
StatusOr<ChannelSource> TCPChannel<MessageTy>::Listen() {
  DCHECK(!this->channel_);
  this->channel_ = std::make_unique<TCPServerChannel>();
  TCPServerChannel* server_channel =
      this->channel_->ToTCPChannelBase()->ToTCPServerChannel();
  return server_channel->Listen();
}

template <typename MessageTy>
void TCPChannel<MessageTy>::DoAcceptLoop(
    TCPServerChannel::AcceptCallback accept_callback) {
  DCHECK(this->channel_);
  DCHECK(!accept_callback.is_null());
  this->channel_->ToTCPChannelBase()->ToTCPServerChannel()->DoAcceptLoop(
      accept_callback);
}

template <typename MessageTy>
void TCPChannel<MessageTy>::AcceptOnce(
    TCPServerChannel::AcceptOnceCallback accept_once_callback) {
  DCHECK(this->channel_);
  DCHECK(!accept_once_callback.is_null());
  this->channel_->ToTCPChannelBase()->ToTCPServerChannel()->AcceptOnce(
      std::move(accept_once_callback));
}

template <typename MessageTy>
void TCPChannel<MessageTy>::Connect(const ChannelSource& channel_source,
                                    StatusCallback callback) {
  DCHECK(!this->channel_);
  DCHECK(!callback.is_null());
  ::net::IPEndPoint ip_endpoint;
  bool ret = ToNetIPEndPoint(channel_source, &ip_endpoint);
  DCHECK(ret);
  this->channel_ = std::make_unique<TCPClientChannel>();
  this->channel_->ToTCPChannelBase()->ToTCPClientChannel()->Connect(
      ip_endpoint, std::move(callback));
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_