#ifndef FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_SOCKET_H_

#include "felicia/core/channel/socket/socket.h"

namespace felicia {

class UnixDomainClientSocket;
class UnixDomainServerSocket;

class UnixDomainSocket : public Socket {
 public:
  UnixDomainSocket();
  ~UnixDomainSocket();

  bool IsUnixDomainSocket() const override;

  UnixDomainClientSocket* ToUnixDomainClientSocket();
  UnixDomainServerSocket* ToUnixDomainServerSocket();

  DISALLOW_COPY_AND_ASSIGN(UnixDomainSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UNIX_DOMAIN_SOCKET_H_