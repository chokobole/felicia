#ifndef FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_H_

#include "gtest/gtest_prod.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/http/http_status_code.h"
#include "third_party/chromium/net/socket/tcp_socket.h"

#include "felicia/core/channel/socket/socket.h"
#include "felicia/core/channel/socket/web_socket_extension.h"
#include "felicia/core/lib/error/statusor.h"

namespace felicia {

class WebSocketServer;

class EXPORT WebSocket : public Socket {
 public:
  class HandshakeHandler {
   public:
    HandshakeHandler(WebSocket* websocket);
    ~HandshakeHandler();

    void Handle(std::unique_ptr<::net::TCPSocket> socket);

   private:
    FRIEND_TEST(WebSocketHandshakeHandler, BasicNegotiate);

    void ReadHeader();
    void OnReadHeader(int result);

    bool Parse();
    bool Validate();
    void SendOK(const std::string& key, const std::string& extension);
    void SendError(::net::HttpStatusCode code);

    void WriteResponse(std::unique_ptr<std::string> response);
    void OnWriteResponse(int result);

    WebSocket* websocket_;  // not owned
    std::unique_ptr<::net::TCPSocket> socket_;
    scoped_refptr<::net::GrowableIOBuffer> buffer_;
    Status status_;
    ::base::CancelableOnceClosure timeout_;
    ::base::flat_map<std::string, std::string> headers_;
    WebSocketExtension extension_;

    DISALLOW_COPY_AND_ASSIGN(HandshakeHandler);
  };

  WebSocket();
  ~WebSocket() override;

  bool IsWebSocket() const override;

  WebSocketServer* ToWebSocketServer();

  virtual void OnHandshaked(
      StatusOr<std::unique_ptr<::net::TCPSocket>> status_or) = 0;

  DISALLOW_COPY_AND_ASSIGN(WebSocket);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_H_