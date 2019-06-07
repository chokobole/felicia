#ifndef FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_SERVER_H_
#define FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_SERVER_H_

#include "third_party/chromium/net/websockets/websocket_deflater.h"

#include "felicia/core/channel/socket/permessage_deflate.h"
#include "felicia/core/channel/socket/tcp_server_socket.h"
#include "felicia/core/channel/socket/web_socket.h"

namespace felicia {

class EXPORT WebSocketServer : public WebSocket {
 public:
  WebSocketServer(const channel::WSSettings& settings);
  ~WebSocketServer();

  // Socket methods
  bool IsServer() const override;

  bool HasReceivers() const;

  StatusOr<ChannelDef> Listen();

  void AcceptLoop(TCPServerSocket::AcceptCallback callback);

  // ChannelImpl methods
  void Write(scoped_refptr<::net::IOBuffer> buffer, int size,
             StatusOnceCallback callback) override;
  void Read(scoped_refptr<::net::GrowableIOBuffer> buffer, int size,
            StatusOnceCallback callback) override;

  // WebSocket methods
  void OnHandshaked(
      StatusOr<std::unique_ptr<::net::TCPSocket>> status_or) override;

  ::net::WebSocketDeflater* deflater();

 private:
  void DoAcceptOnce();
  void OnAccept(StatusOr<std::unique_ptr<::net::TCPSocket>> status_or);

  TCPServerSocket::AcceptCallback accept_callback_;
  std::unique_ptr<TCPServerSocket> tcp_server_socket_;

  WebSocket::HandshakeHandler handshake_handler_;
  std::unique_ptr<::net::WebSocketDeflater> deflater_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_SERVER_H_