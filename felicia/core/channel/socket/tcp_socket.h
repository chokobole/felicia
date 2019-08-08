#ifndef FELICIA_CORE_CHANNEL_SOCKET_TCP_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_TCP_SOCKET_H_

#include "third_party/chromium/net/socket/tcp_socket.h"

#include "felicia/core/channel/socket/stream_socket.h"

namespace felicia {

class TCPClientSocket;
class TCPServerSocket;

class TCPSocket : public StreamSocket {
 public:
  TCPSocket();
  explicit TCPSocket(std::unique_ptr<net::TCPSocket> socket);
  ~TCPSocket() override;

  // Socket methods
  bool IsTCPSocket() const override;
  int Write(net::IOBuffer* buf, int buf_len,
            net::CompletionOnceCallback callback) override;
  int Read(net::IOBuffer* buf, int buf_len,
           net::CompletionOnceCallback callback) override;
  void Close() override;

  TCPClientSocket* ToTCPClientSocket();
  TCPServerSocket* ToTCPServerSocket();

 protected:
  std::unique_ptr<net::TCPSocket> socket_;

  DISALLOW_COPY_AND_ASSIGN(TCPSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_SOCKET_H_