#ifndef FELICIA_CORE_CHANNEL_SOCKET_UDP_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_UDP_SOCKET_H_

#include "third_party/chromium/net/socket/udp_socket.h"

#include "felicia/core/channel/socket/socket.h"

namespace felicia {

class UDPClientSocket;
class UDPServerSocket;

class EXPORT UDPSocket : public Socket {
 public:
  UDPSocket();
  ~UDPSocket() override;

  bool IsUDPSocket() const override;

  UDPClientSocket* ToUDPClientSocket();
  UDPServerSocket* ToUDPServerSocket();

  void OnRead(int result);
  void OnWrite(int result);

  DISALLOW_COPY_AND_ASSIGN(UDPSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_UDP_SOCKET_H_