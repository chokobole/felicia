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

  ChannelDef::Type type() const override { return ChannelDef::CHANNEL_TYPE_WS; }

  bool HasReceivers() const override;

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override {
    NOTREACHED();
  }

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(TCPServerSocket::AcceptCallback accept_callback);

 private:
  void WriteImpl(const std::string& text,
                 SendMessageCallback callback) override;
  void ReadImpl(MessageTy* message, StatusOnceCallback callback) override;

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
void WSChannel<MessageTy>::WriteImpl(const std::string& text,
                                     SendMessageCallback callback) {
  int to_send = text.length();
  this->send_buffer_.SetEnoughCapacityIfDynamic(to_send);

  memcpy(this->send_buffer_.StartOfBuffer(), text.c_str(), to_send);
  this->send_buffer_.AttachWebSocket(to_send);

  this->is_sending_ = true;
  this->send_callback_ = callback;
  this->channel_impl_->Write(
      this->send_buffer_.buffer(), to_send,
      ::base::BindOnce(&WSChannel<MessageTy>::OnSendMessage,
                       ::base::Unretained(this)));
}

template <typename MessageTy>
void WSChannel<MessageTy>::ReadImpl(MessageTy* message,
                                    StatusOnceCallback callback) {
  NOTREACHED() << "Do not read via WSChannel.";
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_WS_CHANNEL_H_