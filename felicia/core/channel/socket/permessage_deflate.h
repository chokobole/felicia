#ifndef FELICIA_CORE_CHANNEL_SOCKET_PERMESSAGE_DEFLATE_H_
#define FELICIA_CORE_CHANNEL_SOCKET_PERMESSAGE_DEFLATE_H_

#include <stdint.h>

#include "felicia/core/channel/socket/web_socket_extension.h"

namespace felicia {

class PermessageDeflate : public WebSocketExtensionInterface {
 public:
  static const char* kKey;

  bool Negotiate(::base::StringTokenizer& params,
                 std::string* response) override;
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_PERMESSAGE_DEFLATE_H_