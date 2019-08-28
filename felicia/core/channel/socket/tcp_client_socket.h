#ifndef FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_

#include "felicia/core/channel/socket/tcp_socket.h"

namespace felicia {

class EXPORT TCPClientSocket : public TCPSocket {
 public:
  TCPClientSocket();
  explicit TCPClientSocket(std::unique_ptr<net::TCPSocket> socket);
  ~TCPClientSocket();

  void Connect(const net::IPEndPoint& ip_endpoint, StatusOnceCallback callback);

  // Socket methods
  bool IsClient() const override;
  bool IsConnected() const override;

  // ChannelImpl methods
  void WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                  StatusOnceCallback callback) override;
  void ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer, int size,
                 StatusOnceCallback callback) override;

 private:
  void OnWriteCheckingReset(int result);
  void OnReadCheckingClosed(int result);

  DISALLOW_COPY_AND_ASSIGN(TCPClientSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_CLIENT_SOCKET_H_