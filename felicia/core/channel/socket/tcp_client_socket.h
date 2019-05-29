#ifndef FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_

#include "felicia/core/channel/socket/tcp_socket.h"

namespace felicia {

class EXPORT TCPClientSocket : public TCPSocket {
 public:
  TCPClientSocket();
  ~TCPClientSocket();

  void set_socket(std::unique_ptr<::net::TCPSocket> socket);

  bool IsClient() const override;

  bool IsConnected() const override;

  void Connect(const ::net::IPEndPoint& ip_endpoint,
               StatusOnceCallback callback);

  void Write(scoped_refptr<::net::IOBuffer> buffer, int size,
             StatusOnceCallback callback) override;
  void Read(scoped_refptr<::net::GrowableIOBuffer> buffer, int size,
            StatusOnceCallback callback) override;

 private:
  void OnConnect(int result);

  void OnWrite(int result);
  void OnRead(int result);

  StatusOnceCallback connect_callback_;

  std::unique_ptr<::net::TCPSocket> socket_;

  DISALLOW_COPY_AND_ASSIGN(TCPClientSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_