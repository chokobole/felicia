#ifndef FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_STERAM_H_
#define FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_STERAM_H_

#include "third_party/chromium/net/base/completion_once_callback.h"
#include "third_party/chromium/net/base/io_buffer.h"
#include "third_party/chromium/net/websockets/websocket_frame.h"

namespace felicia {

class WebSocketStream {
 public:
  virtual ~WebSocketStream() = default;

  virtual int ReadFrames(
      std::vector<std::unique_ptr<::net::WebSocketFrame>>* frames,
      ::net::CompletionOnceCallback callback) = 0;

  virtual int WriteFrames(
      std::vector<std::unique_ptr<::net::WebSocketFrame>>* frames,
      ::net::CompletionOnceCallback callback) = 0;

  virtual void Close() = 0;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_WEB_SOCKET_STERAM_H_