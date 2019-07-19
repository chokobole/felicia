#ifndef FELICIA_CORE_CHANNEL_SOCKET_STREAM_SOCKET_H_
#define FELICIA_CORE_CHANNEL_SOCKET_STREAM_SOCKET_H_

#include "felicia/core/channel/socket/socket.h"

namespace felicia {

class StreamSocket : public Socket {
 public:
  bool IsStreamSocket() const override { return true; }
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_SOCKET_STREAM_SOCKET_H_