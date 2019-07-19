#ifndef FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_

#include "felicia/core/channel/socket/tcp_socket.h"

namespace felicia {

class TCPClientSocket : public TCPSocket {
 public:
  TCPClientSocket();
  ~TCPClientSocket();

  void set_socket(std::unique_ptr<::net::TCPSocket> socket);

  void Connect(const ::net::IPEndPoint& ip_endpoint,
               StatusOnceCallback callback);

  // Socket methods
  bool IsClient() const override;
  bool IsConnected() const override;

  // ChannelImpl methods
  void Write(scoped_refptr<::net::IOBuffer> buffer, int size,
             StatusOnceCallback callback) override;
  void Read(scoped_refptr<::net::GrowableIOBuffer> buffer, int size,
            StatusOnceCallback callback) override;

 private:
  void OnWrite(int result);
  void OnRead(int result);

  DISALLOW_COPY_AND_ASSIGN(TCPClientSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_