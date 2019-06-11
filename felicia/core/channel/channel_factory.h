#ifndef FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/channel/settings.h"
#include "felicia/core/channel/tcp_channel.h"
#include "felicia/core/channel/udp_channel.h"
#ifndef FEL_WIN_NO_GRPC
#include "felicia/core/channel/ws_channel.h"
#endif
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

class ChannelFactory {
 public:
  template <typename MessageTy>
  static std::unique_ptr<Channel<MessageTy>> NewChannel(
      ChannelDef::Type channel_type,
      const channel::Settings& settings = channel::Settings()) {
    std::unique_ptr<Channel<MessageTy>> channel;
    if (channel_type == ChannelDef::TCP) {
      channel = std::make_unique<TCPChannel<MessageTy>>();
    } else if (channel_type == ChannelDef::UDP) {
      channel = std::make_unique<UDPChannel<MessageTy>>();
    }
#ifndef FEL_WIN_NO_GRPC
    else if (channel_type == ChannelDef::WS) {
      channel = std::make_unique<WSChannel<MessageTy>>(settings.ws_settings);
    }
#endif

    return channel;
  }

  DISALLOW_COPY_AND_ASSIGN(ChannelFactory);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_