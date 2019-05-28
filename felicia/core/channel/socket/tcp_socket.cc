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

}  // namespace felicia