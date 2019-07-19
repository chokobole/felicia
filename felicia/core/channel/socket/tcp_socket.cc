#include "felicia/core/channel/socket/tcp_socket.h"

namespace felicia {

TCPSocket::TCPSocket() = default;
TCPSocket::~TCPSocket() = default;

bool TCPSocket::IsTCPSocket() const { return true; }

TCPClientSocket* TCPSocket::ToTCPClientSocket() {
  DCHECK(IsClient());
  return reinterpret_cast<TCPClientSocket*>(this);
}

TCPServerSocket* TCPSocket::ToTCPServerSocket() {
  DCHECK(IsServer());
  return reinterpret_cast<TCPServerSocket*>(this);
}

int TCPSocket::Write(::net::IOBuffer* buf, int buf_len,
                     ::net::CompletionOnceCallback callback) {
  DCHECK(socket_);
  return socket_->Write(
      buf, buf_len, std::move(callback),
      ::net::DefineNetworkTrafficAnnotation("TCPSocket", "Write"));
}

int TCPSocket::Read(::net::IOBuffer* buf, int buf_len,
                    ::net::CompletionOnceCallback callback) {
  DCHECK(socket_);
  return socket_->Read(buf, buf_len, std::move(callback));
}

void TCPSocket::Close() {
  DCHECK(socket_);
  socket_->Close();
}

}  // namespace felicia