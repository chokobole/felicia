#include "felicia/core/channel/socket/web_socket_server.h"

#include "felicia/core/channel/socket/web_socket_basic_stream.h"
#include "felicia/core/channel/socket/web_socket_deflate_stream.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

WebSocketServer::WebSocketServer(const channel::WSSettings& settings)
    : tcp_server_socket_(std::make_unique<TCPServerSocket>()),
      handshake_handler_(this, settings),
      broadcaster_(&channels_) {}

WebSocketServer::~WebSocketServer() = default;

bool WebSocketServer::HasReceivers() const { return channels_.size() > 0; }

StatusOr<ChannelDef> WebSocketServer::Listen() {
  DCHECK(tcp_server_socket_);
  auto status_or = tcp_server_socket_->Listen();
  if (status_or.ok()) {
    ChannelDef channel_def = status_or.ValueOrDie();
    channel_def.set_type(ChannelDef::CHANNEL_TYPE_WS);
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
  tcp_server_socket_->AcceptOnceIntercept(
      base::BindRepeating(&WebSocketServer::OnAccept, base::Unretained(this)));
}

bool WebSocketServer::IsServer() const { return true; }

bool WebSocketServer::IsConnected() const {
  for (auto& channel : channels_) {
    if (!channel->IsClosedState()) return true;
  }
  return false;
}

void WebSocketServer::WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                                 StatusOnceCallback callback) {
  broadcaster_.Broadcast(std::move(buffer), size, std::move(callback));
}

void WebSocketServer::ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer,
                                int size, StatusOnceCallback callback) {
  NOTREACHED() << "You read data from ServerSocket";
}

void WebSocketServer::OnAccept(
    StatusOr<std::unique_ptr<net::TCPSocket>> status_or) {
  if (status_or.ok()) {
    handshake_handler_.Handle(std::move(status_or.ValueOrDie()));
  } else {
    accept_callback_.Run(status_or.status());
    DoAcceptOnce();
  }
}

void WebSocketServer::OnHandshaked(
    StatusOr<std::unique_ptr<net::TCPSocket>> status_or) {
  if (status_or.ok()) {
    auto connection =
        std::make_unique<TCPSocketAdapter>(std::move(status_or.ValueOrDie()));
    std::unique_ptr<WebSocketStream> stream =
        std::make_unique<WebSocketBasicStream>(std::move(connection));
    auto& extensions = handshake_handler_.accepted_extensions();
    if (extensions.size() > 0) {
      DCHECK(extensions.size() == 1);
      if (extensions[0]->IsPerMessageDeflate()) {
        stream = std::make_unique<WebSocketDeflateStream>(
            std::move(stream), extensions[0]->ToPermessageDeflate());
      }
    }
    channels_.push_back(std::make_unique<WebSocketChannel>(std::move(stream)));
    accept_callback_.Run(Status::OK());
  } else {
    accept_callback_.Run(status_or.status());
  }
  DoAcceptOnce();
}

}  // namespace felicia