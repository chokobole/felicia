#ifndef FELICIA_CORE_CHANNEL_WS_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_WS_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/socket/web_socket_server.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

template <typename MessageTy>
class WSChannel : public Channel<MessageTy> {
 public:
  WSChannel(const channel::WSSettings& settings);
  ~WSChannel();

  bool IsWSChannel() const override { return true; }

  ChannelDef::Type type() const override { return ChannelDef::WS; }

  bool HasReceivers() const override;

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override {
    NOTREACHED();
  }

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(TCPServerSocket::AcceptCallback accept_callback);

 private:
  MessageIoError SerializeToBuffer(const MessageTy& message,
                                   int* to_send) override;

  channel::WSSettings settings_;

  DISALLOW_COPY_AND_ASSIGN(WSChannel);
};

template <typename MessageTy>
WSChannel<MessageTy>::WSChannel(const channel::WSSettings& settings)
    : settings_(settings) {}

template <typename MessageTy>
WSChannel<MessageTy>::~WSChannel() = default;

template <typename MessageTy>
bool WSChannel<MessageTy>::HasReceivers() const {
  DCHECK(this->channel_impl_);
  WebSocketServer* server =
      this->channel_impl_->ToSocket()->ToWebSocket()->ToWebSocketServer();
  return server->HasReceivers();
}

template <typename MessageTy>
StatusOr<ChannelDef> WSChannel<MessageTy>::Listen() {
  DCHECK(!this->channel_impl_);
  this->channel_impl_ = std::make_unique<WebSocketServer>(settings_);
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

template <typename MessageTy>
MessageIoError WSChannel<MessageTy>::SerializeToBuffer(const MessageTy& message,
                                                       int* to_send) {
  std::string text;
  MessageIoError err = MessageIO<MessageTy>::SerializeToString(&message, &text);
  if (err != MessageIoError::OK) return err;

  ::net::WebSocketDeflater* deflater = nullptr;
  if (DeflateTraits<MessageTy>::ShouldDeflate(&message, text)) {
    WebSocketServer* server =
        this->channel_impl_->ToSocket()->ToWebSocket()->ToWebSocketServer();
    deflater = server->deflater();
  }

  return MessageIO<MessageTy>::AttachToBuffer(text, this->send_buffer_,
                                              deflater, to_send);
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_WS_CHANNEL_H_