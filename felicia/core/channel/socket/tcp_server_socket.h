#ifndef FELICIA_CORE_CHANNEL_SOCKET_TCP_SERVER_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_TCP_SERVER_SOCKET_H_

#include "felicia/core/channel/socket/tcp_socket.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

class EXPORT TCPServerSocket : public TCPSocket {
 public:
  using AcceptCallback = ::base::RepeatingCallback<void(const Status& s)>;
  using AcceptOnceInterceptCallback =
      ::base::OnceCallback<void(StatusOr<std::unique_ptr<::net::TCPSocket>>)>;

  TCPServerSocket();
  ~TCPServerSocket();

  const std::vector<std::unique_ptr<::net::TCPSocket>>& accepted_sockets()
      const;

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
  void Write(char* buffer, int size, StatusOnceCallback callback) override;
  void Read(char* buffer, int size, StatusOnceCallback callback) override;

 private:
  int DoAccept();
  void DoAcceptLoop();
  void HandleAccpetResult(int result);
  void OnAccept(int result);

  void OnWrite(::net::TCPSocket* socket, int result);

  void EraseClosedSockets();

  AcceptCallback accept_callback_;
  AcceptOnceInterceptCallback accept_once_intercept_callback_;

  size_t to_write_count_ = 0;
  size_t written_count_ = 0;
  int write_result_ = 0;

  bool has_closed_sockets_ = false;

  std::unique_ptr<::net::TCPSocket> socket_;
  ::net::IPEndPoint accepted_endpoint_;
  std::unique_ptr<::net::TCPSocket> accepted_socket_;
  std::vector<std::unique_ptr<::net::TCPSocket>> accepted_sockets_;

  DISALLOW_COPY_AND_ASSIGN(TCPServerSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_SERVER_SOCKET_H_