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

  void Write(char* buffer, int size, StatusOnceCallback callback) override;
  void Read(char* buffer, int size, StatusOnceCallback callback) override;

 private:
  void OnConnect(int result);

  void OnWrite(int result);
  void OnReadAsync(char* buffer,
                   scoped_refptr<::net::IOBufferWithSize> read_buffer,
                   int result);
  void OnRead(int result);

  StatusOnceCallback connect_callback_;

  std::unique_ptr<::net::TCPSocket> socket_;

  DISALLOW_COPY_AND_ASSIGN(TCPClientSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_