#include "felicia/core/channel/tcp_channel.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/memory/ptr_util.h"

#include "felicia/core/channel/socket/ssl_client_socket.h"
#include "felicia/core/channel/socket/ssl_server_context.h"
#include "felicia/core/channel/socket/tcp_client_socket.h"

namespace felicia {

TCPChannel::TCPChannel(const channel::TCPSettings& settings)
    : settings_(settings) {}

TCPChannel::~TCPChannel() = default;

bool TCPChannel::IsTCPChannel() const { return true; }

ChannelDef::Type TCPChannel::type() const {
  return ChannelDef::CHANNEL_TYPE_TCP;
}

bool TCPChannel::HasReceivers() const {
  DCHECK(channel_impl_);
  TCPServerSocket* server_socket =
      channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  return server_socket->accepted_sockets().size() > 0;
}

StatusOr<ChannelDef> TCPChannel::Listen() {
  DCHECK(!channel_impl_);
  channel_impl_ = std::make_unique<TCPServerSocket>();
  TCPServerSocket* server_socket =
      channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  return server_socket->Listen();
}

void TCPChannel::AcceptLoop(TCPServerSocket::AcceptCallback callback) {
  DCHECK(channel_impl_);
  DCHECK(accept_once_intercept_callback_.is_null());
  DCHECK(accept_callback_.is_null());
  DCHECK(!callback.is_null());
  accept_callback_ = callback;
  DoAcceptLoop();
}

void TCPChannel::AcceptOnceIntercept(AcceptOnceInterceptCallback callback) {
  DCHECK(channel_impl_);
  DCHECK(accept_once_intercept_callback_.is_null());
  DCHECK(accept_callback_.is_null());
  DCHECK(!callback.is_null());
  accept_once_intercept_callback_ = std::move(callback);
  TCPServerSocket* server_socket =
      channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  server_socket->AcceptOnceIntercept(
      base::BindOnce(&TCPChannel::OnAccept, base::Unretained(this)));
}

void TCPChannel::AddClientChannel(std::unique_ptr<TCPChannel> channel) {
  DCHECK(channel_impl_);
  TCPServerSocket* server_socket =
      channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  ChannelImpl* channel_impl = channel->channel_impl_.release();
  Socket* socket = channel_impl->ToSocket();
  if (socket->IsTCPSocket()) {
    std::unique_ptr<TCPClientSocket> client_socket;
    client_socket.reset(socket->ToTCPSocket()->ToTCPClientSocket());
    server_socket->AddSocket(std::move(client_socket));
  } else {
#if !defined(FEL_NO_SSL)
    std::unique_ptr<SSLServerSocket> client_socket;
    client_socket.reset(socket->ToSSLSocket()->ToSSLServerSocket());
    server_socket->AddSocket(std::move(client_socket));
#else
    NOTREACHED();
#endif  // !defined(FEL_NO_SSL)
  }
}

void TCPChannel::DoAcceptLoop() {
  TCPServerSocket* server_socket =
      channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
  server_socket->AcceptOnceIntercept(
      base::BindOnce(&TCPChannel::OnAccept, base::Unretained(this)));
}

void TCPChannel::OnAccept(StatusOr<std::unique_ptr<net::TCPSocket>> status_or) {
  if (status_or.ok()) {
    std::unique_ptr<TCPClientSocket> client_socket =
        std::make_unique<TCPClientSocket>(std::move(status_or.ValueOrDie()));
#if !defined(FEL_NO_SSL)
    if (settings_.use_ssl) {
      DCHECK(!ssl_server_socket_);
      DCHECK(settings_.ssl_server_context);
      ssl_server_socket_ = settings_.ssl_server_context->CreateSSLServerSocket(
          std::move(client_socket));
      ssl_server_socket_->Handshake(
          base::BindOnce(&TCPChannel::OnSSLHandshake, base::Unretained(this)));
    } else {
#endif
      if (!accept_callback_.is_null()) {
        TCPServerSocket* server_socket =
            channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
        server_socket->AddSocket(std::move(client_socket));
        accept_callback_.Run(Status::OK());
      } else {
        auto channel = base::WrapUnique(new TCPChannel());
        channel->channel_impl_ = std::move(client_socket);
        std::move(accept_once_intercept_callback_).Run(std::move(channel));
      }
#if !defined(FEL_NO_SSL)
    }
#endif
  } else {
    if (!accept_callback_.is_null()) {
      accept_callback_.Run(status_or.status());
      DoAcceptLoop();
    } else {
      std::move(accept_once_intercept_callback_).Run(status_or.status());
    }
  }
}

#if !defined(FEL_NO_SSL)
void TCPChannel::OnSSLHandshake(const Status& s) {
  if (s.ok()) {
    TCPServerSocket* server_socket =
        channel_impl_->ToSocket()->ToTCPSocket()->ToTCPServerSocket();
    if (!accept_callback_.is_null()) {
      server_socket->AddSocket(std::move(ssl_server_socket_));
    } else {
      auto channel = base::WrapUnique(new TCPChannel());
      channel->channel_impl_ = std::move(ssl_server_socket_);
      std::move(accept_once_intercept_callback_).Run(std::move(channel));
      return;
    }
  } else {
    ssl_server_socket_.reset();
  }
  if (!accept_callback_.is_null()) {
    accept_callback_.Run(s);
    DoAcceptLoop();
  } else {
    std::move(accept_once_intercept_callback_).Run(s);
  }
}
#endif  // !defined(FEL_NO_SSL)

void TCPChannel::Connect(const ChannelDef& channel_def,
                         StatusOnceCallback callback) {
  DCHECK(!channel_impl_);
  DCHECK(!callback.is_null());
  net::IPEndPoint ip_endpoint;
  Status s = ToNetIPEndPoint(channel_def, &ip_endpoint);
  if (!s.ok()) {
    std::move(callback).Run(s);
    return;
  }

  channel_impl_ = std::make_unique<TCPClientSocket>();
  TCPClientSocket* client_socket =
      channel_impl_->ToSocket()->ToTCPSocket()->ToTCPClientSocket();
  client_socket->Connect(
      ip_endpoint, base::BindOnce(&TCPChannel::OnConnect,
                                  base::Unretained(this), std::move(callback)));
}

void TCPChannel::OnConnect(StatusOnceCallback callback, const Status& s) {
#if !defined(FEL_NO_SSL)
  if (settings_.use_ssl) {
    TCPClientSocket* tcp_client_socket =
        channel_impl_.release()->ToSocket()->ToTCPSocket()->ToTCPClientSocket();
    std::unique_ptr<StreamSocket> stream_socket(tcp_client_socket);
    channel_impl_ = std::make_unique<SSLClientSocket>(std::move(stream_socket));
    SSLClientSocket* ssl_client_socket =
        channel_impl_->ToSocket()->ToSSLSocket()->ToSSLClientSocket();
    ssl_client_socket->Connect(std::move(callback));
  } else {
#endif
    std::move(callback).Run(s);
#if !defined(FEL_NO_SSL)
  }
#endif
}

}  // namespace felicia