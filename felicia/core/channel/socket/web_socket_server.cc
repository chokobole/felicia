#include "felicia/core/channel/socket/web_socket_server.h"

#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

WebSocketServer::WebSocketServer(const channel::WSSettings& settings)
    : tcp_server_socket_(std::make_unique<TCPServerSocket>()),
      handshake_handler_(this, settings) {
  if (settings.permessage_deflate_enabled) {
    deflater_ = std::make_unique<::net::WebSocketDeflater>(
        ::net::WebSocketDeflater::DO_NOT_TAKE_OVER_CONTEXT);

    if (!deflater_->Initialize(settings.server_max_window_bits)) {
      DVLOG(1) << "WebSocket protocol error. "
               << "deflater_->Initialize() returns an error.";
    }
  }
}

WebSocketServer::~WebSocketServer() = default;

bool WebSocketServer::IsServer() const { return true; }

bool WebSocketServer::HasReceivers() const {
  DCHECK(tcp_server_socket_);
  return tcp_server_socket_->accepted_sockets().size() > 0;
}

StatusOr<ChannelDef> WebSocketServer::Listen() {
  DCHECK(tcp_server_socket_);
  auto status_or = tcp_server_socket_->Listen();
  if (status_or.ok()) {
    ChannelDef channel_def = status_or.ValueOrDie();
    channel_def.set_type(ChannelDef::WS);
    return channel_def;
  }

  return status_or;
}

void WebSocketServer::AcceptLoop(TCPServerSocket::AcceptCallback callback) {
  DCHECK(!callback.is_null());
  accept_callback_ = callback;
  DoAcceptOnce();
}

void WebSocketServer::DoAcceptOnce() {
  tcp_server_socket_->AcceptOnceIntercept(::base::BindRepeating(
      &WebSocketServer::OnAccept, ::base::Unretained(this)));
}

void WebSocketServer::Write(scoped_refptr<::net::IOBuffer> buffer, int size,
                            StatusOnceCallback callback) {
  tcp_server_socket_->Write(buffer, size, std::move(callback));
}

void WebSocketServer::Read(scoped_refptr<::net::GrowableIOBuffer> buffer,
                           int size, StatusOnceCallback callback) {
  NOTREACHED() << "You read data from ServerSocket";
}

void WebSocketServer::OnAccept(
    StatusOr<std::unique_ptr<::net::TCPSocket>> status_or) {
  if (status_or.ok()) {
    handshake_handler_.Handle(std::move(status_or.ValueOrDie()));
  } else {
    accept_callback_.Run(status_or.status());
    DoAcceptOnce();
  }
}

void WebSocketServer::OnHandshaked(
    StatusOr<std::unique_ptr<::net::TCPSocket>> status_or) {
  if (status_or.ok()) {
    tcp_server_socket_->AddSocket(std::move(status_or.ValueOrDie()));
    accept_callback_.Run(Status::OK());
  } else {
    accept_callback_.Run(status_or.status());
  }
  DoAcceptOnce();
}

::net::WebSocketDeflater* WebSocketServer::deflater() {
  return deflater_.get();
}

}  // namespace felicia