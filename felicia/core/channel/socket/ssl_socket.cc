#include "felicia/core/channel/socket/ssl_socket.h"

namespace felicia {

SSLSocket::SSLSocket() = default;
SSLSocket::~SSLSocket() = default;

bool SSLSocket::IsSSLSocket() const { return true; }

SSLServerSocket* SSLSocket::ToSSLServerSocket() {
  DCHECK(IsServer());
  return reinterpret_cast<SSLServerSocket*>(this);
}

}  // namespace felicia