#include "felicia/core/channel/socket/web_socket_server.h"

#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

WebSocketServer::WebSocketServer()
    : tcp_server_socket_(std::make_unique<TCPServerSocket>()),
      handshake_handler_(this) {}

WebSocketServer::~WebSocketServer() = default;

bool WebSocketServer::IsServer() const { return true; }

StatusOr<ChannelDef> WebSocketServer::Listen() {
  auto status_or = tcp_server_socket_->Listen();
  if (status_or.ok()) {
    ChannelDef channel_def = status_or.ValueOrDie();
    channel_def.set_type(ChannelDef::WS);
    return channel_def;
  }

  return status_or;
}

void WebSocketServer::AcceptLoop(TCPServerSocket::AcceptCallback callback) {
  accept_callback_ = callback;
  DoAcceptLoop();
}

void WebSocketServer::DoAcceptLoop() {
  tcp_server_socket_->AcceptOnceIntercept(::base::BindRepeating(
      &WebSocketServer::OnAccept, ::base::Unretained(this)));
}

void WebSocketServer::Write(char* buffer, int size,
                            StatusOnceCallback callback) {}

void WebSocketServer::Read(char* buffer, int size,
                           StatusOnceCallback callback) {}

void WebSocketServer::OnAccept(
    StatusOr<std::unique_ptr<::net::TCPSocket>> status_or) {
  if (status_or.ok()) {
    handshake_handler_.Handle(std::move(status_or.ValueOrDie()));
  } else {
    accept_callback_.Run(status_or.status());
    DoAcceptLoop();
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
  DoAcceptLoop();
}

}  // namespace felicia