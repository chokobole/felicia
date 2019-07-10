#ifndef FELICIA_CORE_CHANNEL_SOCKET_TCP_SERVER_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_TCP_SERVER_SOCKET_H_

#include "felicia/core/channel/socket/socket_broadcaster.h"
#include "felicia/core/channel/socket/tcp_socket.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

class TCPSocketInterface : public SocketBroadcaster::SocketInterface {
 public:
  TCPSocketInterface(std::unique_ptr<::net::TCPSocket> socket);
  TCPSocketInterface(TCPSocketInterface&& other);
  void operator=(TCPSocketInterface&& other);

  bool IsConnected() override;
  int Write(::net::IOBuffer* buf, int buf_len,
            ::net::CompletionOnceCallback callback) override;
  void Close() override;

 private:
  std::unique_ptr<::net::TCPSocket> socket_;

  DISALLOW_COPY_AND_ASSIGN(TCPSocketInterface);
};

class TCPServerSocket : public TCPSocket {
 public:
  using AcceptCallback = ::base::RepeatingCallback<void(const Status& s)>;
  using AcceptOnceInterceptCallback =
      ::base::OnceCallback<void(StatusOr<std::unique_ptr<::net::TCPSocket>>)>;

  TCPServerSocket();
  ~TCPServerSocket();

  const std::vector<std::unique_ptr<SocketBroadcaster::SocketInterface>>&
  accepted_sockets() const;

  bool IsServer() const override;

  bool IsConnected() const override;

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(AcceptCallback callback);
  void AcceptOnceIntercept(AcceptOnceInterceptCallback callback);

  void AddSocket(std::unique_ptr<::net::TCPSocket> socket);

  // Write the |buffer| to the |accepted_sockets_|. If it succeeds to write
  // all the sockets, then callback with Status::OK(), otherwise callback
  // with the |write_result_|, which is recorded at every time finishing
  // write.
  void Write(scoped_refptr<::net::IOBuffer> buffer, int size,
             StatusOnceCallback callback) override;
  void Read(scoped_refptr<::net::GrowableIOBuffer> buffer, int size,
            StatusOnceCallback callback) override;

 private:
  int DoAccept();
  void DoAcceptLoop();
  void HandleAccpetResult(int result);
  void OnAccept(int result);

  void OnWrite(const Status& s);

  AcceptCallback accept_callback_;
  AcceptOnceInterceptCallback accept_once_intercept_callback_;

  std::unique_ptr<::net::TCPSocket> socket_;
  ::net::IPEndPoint accepted_endpoint_;
  std::unique_ptr<::net::TCPSocket> accepted_socket_;
  std::vector<std::unique_ptr<SocketBroadcaster::SocketInterface>>
      accepted_sockets_;

  SocketBroadcaster broadcaster_;

  DISALLOW_COPY_AND_ASSIGN(TCPServerSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_SERVER_SOCKET_H_