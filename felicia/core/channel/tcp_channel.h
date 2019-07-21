#ifndef FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_
#define FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_

#include "felicia/core/channel/channel.h"
#include "felicia/core/channel/socket/ssl_server_context.h"
#include "felicia/core/channel/socket/ssl_server_socket.h"
#include "felicia/core/channel/socket/tcp_client_socket.h"
#include "felicia/core/channel/socket/tcp_server_socket.h"

namespace felicia {

template <typename MessageTy>
class TCPChannel : public Channel<MessageTy> {
 public:
  using AcceptOnceInterceptCallback = ::base::OnceCallback<void(
      StatusOr<std::unique_ptr<TCPChannel<MessageTy>>>)>;

  TCPChannel(const channel::TCPSettings& settings = channel::TCPSettings());
  ~TCPChannel();

  bool IsTCPChannel() const override { return true; }

  ChannelDef::Type type() const override {
    return ChannelDef::CHANNEL_TYPE_TCP;
  }

  bool HasReceivers() const override;

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(TCPServerSocket::AcceptCallback callback);

  void AcceptOnceIntercept(AcceptOnceInterceptCallback callback);

  void Connect(const ChannelDef& channel_def,
               StatusOnceCallback callback) override;

 private:
  void DoAcceptLoop();

  // This is callback called from AcceptLoop
  void OnAccept(StatusOr<std::unique_ptr<::net::TCPSocket>> status_or);
  // This is callback called from AcceptOnceIntercept
  void OnAccept2(AcceptOnceInterceptCallback callback,
                 StatusOr<std::unique_ptr<::net::TCPSocket>> status_or);

#if !defined(FEL_NO_SSL)
  void OnHandshake(const Status& s);
#endif

  void OnConnect(StatusOnceCallback callback, const Status& s);

  channel::TCPSettings settings_;
  TCPServerSocket::AcceptCallback accept_callback_;

#if !defined(FEL_NO_SSL)
  std::unique_ptr<SSLServerSocket> ssl_server_socket_;
#endif

  DISALLOW_COPY_AND_ASSIGN(TCPChannel);
};

template <typename MessageTy>
TCPChannel<MessageTy>::TCPChannel(const channel::TCPSettings& settings)
    : settings_(settings) {}

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
  accept_callback_ = callback;
  DoAcceptLoop();
}

template <typename MessageTy>
void TCPChannel<MessageTy>::AcceptOnceIntercept(
    AcceptOnceInterceptCallback callback) {
  DCHECK(this->channel_impl_);
  DCHECK(!callback.is_null());
  TCPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  server_socket->AcceptOnceIntercept(
      ::base::BindOnce(&TCPChannel<MessageTy>::OnAccept2,
                       ::base::Unretained(this), std::move(callback)));
}

template <typename MessageTy>
void TCPChannel<MessageTy>::DoAcceptLoop() {
  TCPServerSocket* server_socket =
      this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  server_socket->AcceptOnceIntercept(::base::BindOnce(
      &TCPChannel<MessageTy>::OnAccept, ::base::Unretained(this)));
}

template <typename MessageTy>
void TCPChannel<MessageTy>::OnAccept(
    StatusOr<std::unique_ptr<::net::TCPSocket>> status_or) {
  DCHECK(!accept_callback_.is_null());
  if (status_or.ok()) {
    std::unique_ptr<::net::TCPSocket> socket =
        std::move(status_or.ValueOrDie());
#if !defined(FEL_NO_SSL)
    if (settings_.use_ssl) {
      DCHECK(!ssl_server_socket_);
      DCHECK(settings_.ssl_server_context);
      ssl_server_socket_ = settings_.ssl_server_context->CreateSSLServerSocket(
          std::make_unique<TCPClientSocket>(std::move(socket)));
      ssl_server_socket_->Handshake(::base::BindOnce(
          &TCPChannel<MessageTy>::OnHandshake, ::base::Unretained(this)));
      return;
    } else {
#endif
      TCPServerSocket* server_socket =
          this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
      server_socket->AddSocket(std::move(socket));
      accept_callback_.Run(Status::OK());
#if !defined(FEL_NO_SSL)
    }
#endif
  } else {
    accept_callback_.Run(status_or.status());
  }
  DoAcceptLoop();
}

#if !defined(FEL_NO_SSL)
template <typename MessageTy>
void TCPChannel<MessageTy>::OnHandshake(const Status& s) {
  DCHECK(!accept_callback_.is_null());
  if (s.ok()) {
    TCPServerSocket* server_socket =
        this->channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
    server_socket->AddSocket(std::move(ssl_server_socket_));
  } else {
    ssl_server_socket_.reset();
  }
  accept_callback_.Run(s);
  DoAcceptLoop();
}
#endif

template <typename MessageTy>
void TCPChannel<MessageTy>::OnAccept2(
    AcceptOnceInterceptCallback callback,
    StatusOr<std::unique_ptr<::net::TCPSocket>> status_or) {
  if (status_or.ok()) {
    auto channel = std::make_unique<TCPChannel<MessageTy>>();
    channel->channel_impl_ =
        std::make_unique<TCPClientSocket>(std::move(status_or.ValueOrDie()));
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
  client_socket->Connect(
      ip_endpoint,
      ::base::BindOnce(&TCPChannel<MessageTy>::OnConnect,
                       ::base::Unretained(this), std::move(callback)));
}

template <typename MessageTy>
void TCPChannel<MessageTy>::OnConnect(StatusOnceCallback callback,
                                      const Status& s) {
  std::move(callback).Run(s);
}

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_TCP_CHANNEL_H_