#ifndef FELIICA_CORE_CHANNEL_SOCKET_SSL_SOCKET_H_
#define FELIICA_CORE_CHANNEL_SOCKET_SSL_SOCKET_H_

#include "felicia/core/channel/socket/stream_socket.h"

namespace felicia {

class SSLServerSocket;

class SSLSocket : public StreamSocket {
 public:
  SSLSocket();
  ~SSLSocket() override;

  bool IsSSLSocket() const override;

  SSLServerSocket* ToSSLServerSocket();

  DISALLOW_COPY_AND_ASSIGN(SSLSocket);
};

}  // namespace felicia

#endif  // FELIICA_CORE_CHANNEL_SOCKET_SSL_SOCKET_H_
