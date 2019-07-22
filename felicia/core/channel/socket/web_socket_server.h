#ifndef FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_SERVER_H_
#define FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_SERVER_H_

#include "felicia/core/channel/socket/tcp_server_socket.h"
#include "felicia/core/channel/socket/web_socket.h"
#include "felicia/core/channel/socket/web_socket_channel.h"
#include "felicia/core/channel/socket/web_socket_channel_broadcaster.h"

namespace felicia {

class EXPORT WebSocketServer : public WebSocket {
 public:
  WebSocketServer(const channel::WSSettings& settings);
  ~WebSocketServer();

  bool HasReceivers() const;

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(TCPServerSocket::AcceptCallback callback);

  // Socket methods
  bool IsServer() const override;
  bool IsConnected() const override;

  // ChannelImpl methods
  void Write(scoped_refptr<::net::IOBuffer> buffer, int size,
             StatusOnceCallback callback) override;
  void Read(scoped_refptr<::net::GrowableIOBuffer> buffer, int size,
            StatusOnceCallback callback) override;

  // WebSocket methods
  void OnHandshaked(
      StatusOr<std::unique_ptr<::net::TCPSocket>> status_or) override;

 private:
  void DoAcceptOnce();
  void OnAccept(StatusOr<std::unique_ptr<::net::TCPSocket>> status_or);

  TCPServerSocket::AcceptCallback accept_callback_;
  std::unique_ptr<TCPServerSocket> tcp_server_socket_;

  WebSocket::HandshakeHandler handshake_handler_;
  std::vector<std::unique_ptr<WebSocketChannel>> channels_;
  WebSocketChannelBroadcaster broadcaster_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_SERVER_H_