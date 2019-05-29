#include "felicia/core/channel/socket/web_socket_server.h"

#include "third_party/chromium/net/websockets/websocket_frame.h"

#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

WebSocketServer::WebSocketServer()
    : tcp_server_socket_(std::make_unique<TCPServerSocket>()),
      handshake_handler_(this) {}

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
  auto frame = std::make_unique<::net::WebSocketFrame>(
      ::net::WebSocketFrameHeader::kOpCodeText);
  ::net::WebSocketFrameHeader& header = frame->header;
  header.final = true;
  header.masked = false;
  header.payload_length = size;
  frame->data = std::move(buffer);

  int total_size = ::net::GetWebSocketFrameHeaderSize(frame->header) +
                   frame->header.payload_length;
  scoped_refptr<::net::IOBuffer> write_buffer =
      ::base::MakeRefCounted<::net::IOBufferWithSize>(total_size);
  int header_size = ::net::WriteWebSocketFrameHeader(
      frame->header, nullptr, write_buffer->data(), total_size);
  memcpy(write_buffer->data() + header_size, frame->data->data(),
         total_size - header_size);

  tcp_server_socket_->Write(write_buffer, total_size, std::move(callback));
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

}  // namespace felicia