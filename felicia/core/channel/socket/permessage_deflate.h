#ifndef FELICIA_CORE_CHANNEL_SOCKET_PERMESSAGE_DEFLATE_H_
#define FELICIA_CORE_CHANNEL_SOCKET_PERMESSAGE_DEFLATE_H_

#include <stdint.h>

#include "third_party/chromium/net/websockets/websocket_deflater.h"

#include "felicia/core/channel/socket/web_socket_extension.h"

namespace felicia {

class PermessageDeflate : public WebSocketExtensionInterface {
 public:
  static constexpr const char* kKey = "permessage-deflate";
  static constexpr const int kMinWindowBits = 8;
  static constexpr const int kMaxWindowBits = 15;

  bool Negotiate(::base::StringTokenizer& params,
                 const channel::WSSettings& settings,
                 std::string* response) override;

  void AppendResponse(std::string* response) const override;

  bool IsPerMessageDeflate() const override { return true; }

  ::net::WebSocketDeflater::ContextTakeOverMode client_context_take_over_mode()
      const {
    return client_context_take_over_mode_;
  }

  ::net::WebSocketDeflater::ContextTakeOverMode server_context_take_over_mode()
      const {
    return server_context_take_over_mode_;
  }

  int client_max_window_bits() const { return client_max_window_bits_; }

  int server_max_window_bits() const { return server_max_window_bits_; }

 private:
  ::net::WebSocketDeflater::ContextTakeOverMode client_context_take_over_mode_;
  ::net::WebSocketDeflater::ContextTakeOverMode server_context_take_over_mode_;
  int client_max_window_bits_;
  int server_max_window_bits_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_PERMESSAGE_DEFLATE_H_