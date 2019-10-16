#include "felicia/core/rpc/server_interface.h"

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/net/net_util.h"

namespace felicia {
namespace rpc {

ServerInterface::ServerInterface() = default;
ServerInterface::~ServerInterface() = default;

ChannelDef ServerInterface::channel_def() const {
  return ToChannelDef(
      net::IPEndPoint(HostIPAddress(HOST_IP_ONLY_ALLOW_IPV4), port_),
      ChannelDef::CHANNEL_TYPE_TCP);
}

std::string ServerInterface::ConfigureServerAddress() {
  port_ = PickRandomPort(true);
  return base::StringPrintf("[::]:%d", static_cast<int>(port_));
}

}  // namespace rpc
}  // namespace felicia