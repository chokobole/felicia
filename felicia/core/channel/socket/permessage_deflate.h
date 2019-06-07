#ifndef FELICIA_CORE_CHANNEL_SOCKET_PERMESSAGE_DEFLATE_H_
#define FELICIA_CORE_CHANNEL_SOCKET_PERMESSAGE_DEFLATE_H_

#include <stdint.h>

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

 private:
  bool client_no_context_takeover_;
  bool server_no_context_takeover_;
  uint8_t client_max_window_bits_;
  uint8_t server_max_window_bits_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_PERMESSAGE_DEFLATE_H_