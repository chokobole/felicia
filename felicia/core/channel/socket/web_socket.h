#ifndef FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/http/http_status_code.h"
#include "third_party/chromium/net/socket/tcp_socket.h"

#include "felicia/core/channel/socket/socket.h"
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
    void ReadHeader();
    void OnReadHeader(scoped_refptr<::net::IOBuffer> buffer, int result);

    void Parse();
    void Validate();
    void SendOK(const std::string& key);
    void SendError(::net::HttpStatusCode code);

    void WriteResponse(const std::string& response);
    void OnWriteResponse(int result);

    WebSocket* websocket_;  // not owned
    std::unique_ptr<::net::TCPSocket> socket_;
    std::unique_ptr<std::vector<char>> buffer_;
    int read_;
    Status status_;
    ::base::CancelableOnceClosure timeout_;
    ::base::flat_map<std::string, std::string> headers_;

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