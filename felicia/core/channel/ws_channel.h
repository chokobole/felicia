#ifndef FELICIA_CORE_CHANNEL_WS_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_WS_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/socket/web_socket_server.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

template <typename MessageTy>
class WSChannel : public Channel<MessageTy> {
 public:
  WSChannel();
  ~WSChannel();

  bool IsWSChannel() const override { return true; }

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override {
    NOTREACHED();
  }

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(TCPServerSocket::AcceptCallback accept_callback);

 private:
  DISALLOW_COPY_AND_ASSIGN(WSChannel);
};

template <typename MessageTy>
WSChannel<MessageTy>::WSChannel() {}

template <typename MessageTy>
WSChannel<MessageTy>::~WSChannel() = default;

template <typename MessageTy>
StatusOr<ChannelDef> WSChannel<MessageTy>::Listen() {
  DCHECK(!this->channel_impl_);
  this->channel_impl_ = std::make_unique<WebSocketServer>();
  WebSocketServer* server =
      this->channel_impl_->ToSocket()->ToWebSocket()->ToWebSocketServer();
  return server->Listen();
}

template <typename MessageTy>
void WSChannel<MessageTy>::AcceptLoop(
    TCPServerSocket::AcceptCallback accept_callback) {
  DCHECK(this->channel_impl_);
  DCHECK(!accept_callback.is_null());
  WebSocketServer* server =
      this->channel_impl_->ToSocket()->ToWebSocket()->ToWebSocketServer();
  server->AcceptLoop(accept_callback);
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_WS_CHANNEL_H_