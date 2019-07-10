#include "felicia/core/channel/socket/unix_domain_socket.h"

namespace felicia {

UnixDomainSocket::UnixDomainSocket() = default;

UnixDomainSocket::~UnixDomainSocket() = default;

bool UnixDomainSocket::IsUnixDomainSocket() const { return true; }

UnixDomainClientSocket* UnixDomainSocket::ToUnixDomainClientSocket() {
  DCHECK(IsClient());
  return reinterpret_cast<UnixDomainClientSocket*>(this);
}

UnixDomainServerSocket* UnixDomainSocket::ToUnixDomainServerSocket() {
  DCHECK(IsServer());
  return reinterpret_cast<UnixDomainServerSocket*>(this);
}

}  // namespace felicia