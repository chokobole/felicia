#ifndef FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/channel/tcp_channel.h"
#include "felicia/core/channel/udp_channel.h"
#include "felicia/core/channel/ws_channel.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

class ChannelFactory {
 public:
  template <typename MessageTy>
  static std::unique_ptr<Channel<MessageTy>> NewChannel(
      ChannelDef::Type channel_type) {
    std::unique_ptr<Channel<MessageTy>> channel;
    if (channel_type == ChannelDef::TCP) {
      channel = std::make_unique<TCPChannel<MessageTy>>();
    } else if (channel_type == ChannelDef::UDP) {
      channel = std::make_unique<UDPChannel<MessageTy>>();
    } else if (channel_type == ChannelDef::WS) {
      channel = std::make_unique<WSChannel<MessageTy>>();
    }

    return channel;
  }

  DISALLOW_COPY_AND_ASSIGN(ChannelFactory);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_