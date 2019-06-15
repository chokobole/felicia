#ifndef FELICIA_CORE_CHANNEL_SOCKET_TCP_SOCKET_BROADCASTER_H_
#define FELICIA_CORE_CHANNEL_SOCKET_TCP_SOCKET_BROADCASTER_H_

#include "third_party/chromium/net/socket/tcp_socket.h"

#include "felicia/core/lib/error/status.h"

namespace felicia {

class TCPSocketBroadcaster {
 public:
  class SocketInterface {
   public:
    virtual ~SocketInterface();
    virtual bool IsConnected() = 0;
    virtual int Write(::net::IOBuffer* buf, int buf_len,
                      ::net::CompletionOnceCallback callback) = 0;
    virtual void Close() = 0;
  };

  explicit TCPSocketBroadcaster(
      std::vector<std::unique_ptr<SocketInterface>>* sockets);
  ~TCPSocketBroadcaster();

  void Broadcast(scoped_refptr<::net::IOBuffer> buffer, int size,
                 StatusOnceCallback callback);

 private:
  void OnWrite(SocketInterface* socket, int result);

  void EraseClosedSockets();

  StatusOnceCallback callback_;

  size_t to_write_count_ = 0;
  size_t written_count_ = 0;
  int write_result_ = 0;

  bool has_closed_sockets_ = false;

  std::vector<std::unique_ptr<SocketInterface>>* sockets_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_SOCKET_BROADCASTER_H_