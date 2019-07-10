#ifndef FELICIA_CORE_CHANNEL_SOCKET_TCP_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_TCP_SOCKET_H_

#include "third_party/chromium/net/socket/tcp_socket.h"

#include "felicia/core/channel/socket/socket.h"

namespace felicia {

class TCPClientSocket;
class TCPServerSocket;

class TCPSocket : public Socket {
 public:
  TCPSocket();
  ~TCPSocket() override;

  bool IsTCPSocket() const override;

  TCPClientSocket* ToTCPClientSocket();
  TCPServerSocket* ToTCPServerSocket();

  virtual bool IsConnected() const = 0;

  DISALLOW_COPY_AND_ASSIGN(TCPSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_SOCKET_H_