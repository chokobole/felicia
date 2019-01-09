#ifndef FELICIA_CORE_PLATFORM_TCP_SOCKET_H_
#define FELICIA_CORE_PLATFORM_TCP_SOCKET_H_

#include <vector>

#include "felicia/core/lib/base/export.h"
#include "felicia/core/platform/socket.h"

namespace felicia {
namespace net {

class EXPORT TCPSocketBase : public Socket {
 public:
  TCPSocketBase();
  ~TCPSocketBase() override;

  bool IsTCPSocket() const override { return true; }

  // set TCP_NODELAY option. Return OK if succeded, otherwise return error code.
  int SetNoDelay(bool no_delay) MUST_USE_RESULT;

  DISALLOW_COPY_AND_ASSIGN(TCPSocketBase);
};

}  // namespace net
}  // namespace felicia

#include "felicia/core/lib/base/platform.h"
#if defined(PLATFORM_WINDOWS)
#include "felicia/core/platform/win/tcp_socket.h"
#elif defined(PLATFORM_POSIX)
#include "felicia/core/platform/posix/tcp_socket.h"
#else
#error Define the appropriate PLATFORM_<foo> macro for this platform
#endif

#endif  // FELICIA_CORE_PLATFORM_TCP_SOCKET_H_
