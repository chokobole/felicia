#ifndef FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/socket/tcp_client_socket.h"
#include "felicia/core/channel/socket/tcp_server_socket.h"

namespace felicia {

template <typename MessageTy>
class TCPChannel : public Channel<MessageTy> {
 public:
  using AcceptOnceInterceptCallback = ::base::OnceCallback<void(
      StatusOr<std::unique_ptr<TCPChannel<MessageTy>>>)>;

  TCPChannel();
  ~TCPChannel();

  bool IsTCPChannel() const override { return true; }

  ChannelDef::Type type() const override {
    return ChannelDef::CHANNEL_TYPE_TCP;
  }

  bool HasReceivers() const override;

  bool IsConnected() const {
    DCHECK(this->channel_impl_);
    return this->channel_impl_->ToSocket()->ToTCPSocket()->IsConnected();
  }

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(TCPServerSocket::AcceptCallback callback);

  void AcceptOnceIntercept(AcceptOnceInterceptCallback callback);

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

 private:
  void OnAccept(AcceptOnceInterceptCallback callback,
                StatusOr<std::unique_ptr<::net::TCPSocket>> status_or);

  DISALLOW_COPY_AND_ASSIGN(TCPChannel);
};

template <typename MessageTy>
TCPChannel<MessageTy>::TCPChannel() {}

template <typename MessageTy>
TCPChannel<MessageTy>::~TCPChannel() = default;

template <typename MessageTy>
bool TCPChannel<MessageTy>::HasReceivers() const {
  DCHECK(this->channel_impl_);
  TCPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  return server_socket->accepted_sockets().size() > 0;
}

template <typename MessageTy>
StatusOr<ChannelDef> TCPChannel<MessageTy>::Listen() {
  DCHECK(!this->channel_impl_);
  this->channel_impl_ = std::make_unique<TCPServerSocket>();
  TCPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  return server_socket->Listen();
}

template <typename MessageTy>
void TCPChannel<MessageTy>::AcceptLoop(
    TCPServerSocket::AcceptCallback callback) {
  DCHECK(this->channel_impl_);
  DCHECK(!callback.is_null());
  TCPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  server_socket->AcceptLoop(callback);
}

template <typename MessageTy>
void TCPChannel<MessageTy>::AcceptOnceIntercept(
    AcceptOnceInterceptCallback callback) {
  DCHECK(this->channel_impl_);
  DCHECK(!callback.is_null());
  TCPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  server_socket->AcceptOnceIntercept(
      ::base::BindOnce(&TCPChannel<MessageTy>::OnAccept,
                       ::base::Unretained(this), std::move(callback)));
}

template <typename MessageTy>
void TCPChannel<MessageTy>::OnAccept(
    AcceptOnceInterceptCallback callback,
    StatusOr<std::unique_ptr<::net::TCPSocket>> status_or) {
  if (status_or.ok()) {
    auto channel = std::make_unique<TCPChannel<MessageTy>>();
    auto client_socket = std::make_unique<TCPClientSocket>();
    client_socket->set_socket(std::move(status_or.ValueOrDie()));
    channel->channel_impl_ = std::move(client_socket);
    std::move(callback).Run(std::move(channel));
  } else {
    std::move(callback).Run(status_or.status());
  }
}

template <typename MessageTy>
void TCPChannel<MessageTy>::Connect(const ChannelDef& channel_def,
                                    StatusOnceCallback callback) {
  DCHECK(!this->channel_impl_);
  DCHECK(!callback.is_null());
  ::net::IPEndPoint ip_endpoint;
  Status s = ToNetIPEndPoint(channel_def, &ip_endpoint);
  if (!s.ok()) {
    std::move(callback).Run(s);
    return;
  }
  this->channel_impl_ = std::make_unique<TCPClientSocket>();
  TCPClientSocket* client_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPClientSocket();
  client_socket->Connect(ip_endpoint, std::move(callback));
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_