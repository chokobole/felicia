#ifndef FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_
#define FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_

#include <memory>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/channel/settings.h"
#include "felicia/core/channel/tcp_channel.h"
#include "felicia/core/channel/udp_channel.h"
#include "felicia/core/channel/ws_channel.h"
#if defined(OS_POSIX)
#include "felicia/core/channel/uds_channel.h"
#endif
#include "felicia/core/channel/shm_channel.h"
#include "felicia/core/protobuf/channel.pb.h"

namespace felicia {

class ChannelFactory {
 public:
  template <typename MessageTy>
  static std::unique_ptr<Channel<MessageTy>> NewChannel(
      ChannelDef::Type channel_type,
      const channel::Settings& settings = channel::Settings()) {
    std::unique_ptr<Channel<MessageTy>> channel;
    if (channel_type == ChannelDef::CHANNEL_TYPE_TCP) {
      channel = std::make_unique<TCPChannel<MessageTy>>(settings.tcp_settings);
    } else if (channel_type == ChannelDef::CHANNEL_TYPE_UDP) {
      channel = std::make_unique<UDPChannel<MessageTy>>();
    } else if (channel_type == ChannelDef::CHANNEL_TYPE_WS) {
      channel = std::make_unique<WSChannel<MessageTy>>(settings.ws_settings);
    }
#if defined(OS_POSIX)
    else if (channel_type == ChannelDef::CHANNEL_TYPE_UDS) {
      channel = std::make_unique<UDSChannel<MessageTy>>();
    }
#endif
    else if (channel_type == ChannelDef::CHANNEL_TYPE_SHM) {
      channel = std::make_unique<ShmChannel<MessageTy>>(settings.shm_settings);
    }
    channel->set_receive_from_ros(settings.receive_from_ros);

    return channel;
  }

  DISALLOW_COPY_AND_ASSIGN(ChannelFactory);
};

}  // namespace felicia

#endif  // FELICIA_CORE_CHANNEL_CHANNEL_FACTORY_H_