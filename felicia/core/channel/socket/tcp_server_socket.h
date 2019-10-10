#ifndef FELICIA_CORE_CHANNEL_SOCKET_TCP_SERVER_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_TCP_SERVER_SOCKET_H_

#include "felicia/core/channel/socket/stream_socket_broadcaster.h"
#include "felicia/core/channel/socket/tcp_socket.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

class EXPORT TCPServerSocket : public TCPSocket {
 public:
  using AcceptCallback = base::RepeatingCallback<void(const Status& s)>;
  using AcceptOnceInterceptCallback =
      base::OnceCallback<void(StatusOr<std::unique_ptr<net::TCPSocket>>)>;

  TCPServerSocket();
  ~TCPServerSocket();

  const std::vector<std::unique_ptr<StreamSocket>>& accepted_sockets() const;

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(AcceptCallback callback);
  void AcceptOnceIntercept(AcceptOnceInterceptCallback callback);

  void AddSocket(std::unique_ptr<net::TCPSocket> socket);
  void AddSocket(std::unique_ptr<StreamSocket> socket);
  void AddSocket(std::unique_ptr<TCPSocket> socket);

  // Socket methods
  bool IsServer() const override;
  bool IsConnected() const override;

  // ChannelImpl methods
  // Write the |buffer| to the |accepted_sockets_|. If it succeeds to write
  // all the sockets, then callback with Status::OK(), otherwise callback
  // with the |write_result_|, which is recorded at every time finishing
  // write.
  void WriteAsync(scoped_refptr<net::IOBuffer> buffer, int size,
                  StatusOnceCallback callback) override;
  void ReadAsync(scoped_refptr<net::GrowableIOBuffer> buffer, int size,
                 StatusOnceCallback callback) override;

 private:
  int DoAccept();
  void DoAcceptLoop();
  void HandleAccpetResult(int result);
  void OnAccept(int result);

  void OnWrite(const Status& s);

  AcceptCallback accept_callback_;
  AcceptOnceInterceptCallback accept_once_intercept_callback_;

  net::IPEndPoint accepted_endpoint_;
  std::unique_ptr<net::TCPSocket> accepted_socket_;
  std::vector<std::unique_ptr<StreamSocket>> accepted_sockets_;

  StreamSocketBroadcaster broadcaster_;

  DISALLOW_COPY_AND_ASSIGN(TCPServerSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_TCP_SERVER_SOCKET_H_