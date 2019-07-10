#include "felicia/core/channel/socket/socket.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/net/base/net_errors.h"

#include "felicia/core/lib/error/errors.h"

namespace felicia {

Socket::Socket() = default;
Socket::~Socket() = default;

bool Socket::IsSocket() const { return true; }

bool Socket::IsClient() const { return false; }
bool Socket::IsServer() const { return false; }

bool Socket::IsTCPSocket() const { return false; }
bool Socket::IsUDPSocket() const { return false; }
bool Socket::IsWebSocket() const { return false; }
#if defined(OS_POSIX)
bool Socket::IsUnixDomainSocket() const { return false; }
#endif

TCPSocket* Socket::ToTCPSocket() {
  DCHECK(IsTCPSocket());
  return reinterpret_cast<TCPSocket*>(this);
}

UDPSocket* Socket::ToUDPSocket() {
  DCHECK(IsUDPSocket());
  return reinterpret_cast<UDPSocket*>(this);
}

WebSocket* Socket::ToWebSocket() {
  DCHECK(IsWebSocket());
  return reinterpret_cast<WebSocket*>(this);
}

#if defined(OS_POSIX)
UnixDomainSocket* Socket::ToUnixDomainSocket() {
  DCHECK(IsUnixDomainSocket());
  return reinterpret_cast<UnixDomainSocket*>(this);
}
#endif

// static
void Socket::CallbackWithStatus(StatusOnceCallback callback, int result) {
  if (result >= 0) {
    std::move(callback).Run(Status::OK());
  } else {
    std::move(callback).Run(errors::NetworkError(::net::ErrorToString(result)));
  }
}

}  // namespace felicia