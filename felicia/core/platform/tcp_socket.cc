#include "felicia/core/platform/tcp_socket.h"

#include "felicia/core/lib/base/platform.h"
#if defined(PLATFORM_POSIX)
#include <netinet/tcp.h>
#elif defined(PLATFORM_WINDOWS)

#endif

namespace felicia {
namespace net {

TCPSocketBase::TCPSocketBase() = default;
TCPSocketBase::~TCPSocketBase() = default;

int TCPSocketBase::SetNoDelay(bool no_delay) {
  DCHECK(IsValid());

#if defined(PLATFORM_POSIX)
  int on = no_delay ? 1 : 0;
#elif defined(PLATFORM_WINDOWS)
  BOOL on = no_delay ? TRUE : FALSE;
#endif
  int rv = setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY,
                      reinterpret_cast<const char*>(&on), sizeof(on));
  return rv == -1 ? MapSystemError(errno) : OK;
}

}  // namespace net
}  // namespace felicia