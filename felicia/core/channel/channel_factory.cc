#include "felicia/core/channel/channel_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"

namespace felicia {

// static
std::unique_ptr<Channel> ChannelFactory::NewChannel(
    ChannelDef::Type channel_type, const channel::Settings& settings) {
  std::unique_ptr<Channel> channel;
  if (channel_type == ChannelDef::CHANNEL_TYPE_SHM) {
    channel = base::WrapUnique(new ShmChannel(settings.shm_settings));
  } else if (channel_type == ChannelDef::CHANNEL_TYPE_UDP) {
    channel = base::WrapUnique(new UDPChannel());
  }
#if defined(OS_POSIX)
  else if (channel_type == ChannelDef::CHANNEL_TYPE_UDS) {
    channel = base::WrapUnique(new UDSChannel(settings.uds_settings));
  }
#endif
  else if (channel_type == ChannelDef::CHANNEL_TYPE_TCP) {
    channel = base::WrapUnique(new TCPChannel(settings.tcp_settings));
  } else if (channel_type == ChannelDef::CHANNEL_TYPE_WS) {
    channel = base::WrapUnique(new WSChannel(settings.ws_settings));
  }

  return channel;
}

}  // namespace felicia