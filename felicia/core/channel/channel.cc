#include "felicia/core/channel/channel.h"

#include "felicia/core/platform/net_util.h"

namespace felicia {

bool ToNetIPEndPoint(const ChannelSource& channel_source,
                     ::net::IPEndPoint* ip_endpoint) {
  ::net::IPAddress ip;
  if (!ip.AssignFromIPLiteral(channel_source.ip_endpoint().ip())) {
    return false;
  }

  *ip_endpoint = ::net::IPEndPoint(ip, channel_source.ip_endpoint().port());
  return true;
}

ChannelSource ToChannelSource(const ::net::IPEndPoint& ip_endpoint,
                              const ChannelDef channel_def) {
  ChannelSource channel_source;
  IPEndPoint* endpoint = channel_source.mutable_ip_endpoint();
  endpoint->set_ip(ip_endpoint.address().ToString());
  endpoint->set_port(ip_endpoint.port());
  *channel_source.mutable_channel_def() = channel_def;
  return channel_source;
}

ChannelSource PickRandomChannelSource(const ChannelDef channel_def) {
  ChannelSource channel_source;
  IPEndPoint* ip_endpoint = channel_source.mutable_ip_endpoint();
  ip_endpoint->set_ip(
      net::HostIPAddress(net::HOST_IP_ONLY_ALLOW_IPV4).ToString());
  ip_endpoint->set_port(
      net::PickRandomPort(channel_def.type() == ChannelDef_Type_TCP));
  *channel_source.mutable_channel_def() = channel_def;
  return channel_source;
}

}  // namespace felicia