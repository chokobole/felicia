#include "felicia/core/platform/socket.h"

#include "felicia/core/lib/base/platform.h"
#if defined(PLATFORM_POSIX)
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#elif defined(PLATFORM_WINDOWS)

#endif

#include "felicia/core/lib/net/sockaddr_storage.h"

namespace felicia {
namespace net {

int Socket::Bind(const IPEndPoint& endpoint) {
  DCHECK(IsValid());

  SockaddrStorage address;
  if (!endpoint.ToSockAddr(address.addr, &address.addr_len))
    return ERR_ADDRESS_INVALID;

  int rv = bind(fd_, address.addr, address.addr_len);
  if (rv < 0) {
    PLOG(ERROR) << "bind() failed";
    return MapSystemError(errno);
  }

  return OK;
}

int Socket::GetLocalAddress(IPEndPoint* endpoint) const {
  DCHECK(endpoint);

  if (!IsValid()) return ERR_SOCKET_NOT_CONNECTED;

  SockaddrStorage address;
  if (getsockname(fd_, address.addr, &address.addr_len) < 0)
    return MapSystemError(errno);

  if (!endpoint->FromSockAddr(address.addr, address.addr_len))
    return ERR_ADDRESS_INVALID;

  return OK;
}

int Socket::GetPeerAddress(IPEndPoint* endpoint) const {
  DCHECK(endpoint);

  if (!IsValid()) return ERR_SOCKET_NOT_CONNECTED;

  if (!HasPeerAddress()) return ERR_SOCKET_NOT_CONNECTED;

  *endpoint = *peer_address_;
  return OK;
}

void Socket::SetPeerAddress(const IPEndPoint& endpoint) {
  DCHECK(!peer_address_);
  peer_address_.reset(new IPEndPoint(endpoint));
}

bool Socket::HasPeerAddress() const { return peer_address_ != nullptr; }

int Socket::SetKeepAlive(bool keep_alive) {
  DCHECK(IsValid());

#if defined(PLATFORM_POSIX)
  int on = keep_alive ? 1 : 0;
#elif defined(PLATFORM_WINDOWS)
  BOOL on = keep_alive ? TRUE : FALSE;
#endif
  int rv = setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE,
                      reinterpret_cast<const char*>(&on), sizeof(on));
  return rv == -1 ? MapSystemError(errno) : OK;
}

int Socket::SetReuseAddr(bool reuse) {
  DCHECK(IsValid());

#if defined(PLATFORM_POSIX)
  int on = reuse ? 1 : 0;
#elif defined(PLATFORM_WINDOWS)
  BOOL on = reuse ? TRUE : FALSE;
#endif
  int rv = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR,
                      reinterpret_cast<const char*>(&on), sizeof(on));
  return rv == -1 ? MapSystemError(errno) : OK;
}

}  // namespace net
}  // namespace felicia