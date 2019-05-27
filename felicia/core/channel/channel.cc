#include "felicia/core/channel/channel.h"

#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/lib/net/net_util.h"

namespace felicia {

bool ToNetIPEndPoint(const ChannelDef& channel_def,
                     ::net::IPEndPoint* ip_endpoint) {
  ::net::IPAddress ip;
  if (!ip.AssignFromIPLiteral(channel_def.ip_endpoint().ip())) {
    return false;
  }

  *ip_endpoint = ::net::IPEndPoint(ip, channel_def.ip_endpoint().port());
  return true;
}

ChannelDef ToChannelDef(const ::net::IPEndPoint& ip_endpoint,
                        ChannelDef::Type type) {
  DCHECK(type == ChannelDef::TCP || type == ChannelDef::UDP);
  ChannelDef channel_def;
  IPEndPoint* endpoint = channel_def.mutable_ip_endpoint();
  endpoint->set_ip(ip_endpoint.address().ToString());
  endpoint->set_port(ip_endpoint.port());
  channel_def.set_type(type);
  return channel_def;
}

std::string EndPointToString(const ChannelDef& channel_def) {
  if (channel_def.has_ip_endpoint()) {
    ::net::IPEndPoint ip_endpoint;
    if (ToNetIPEndPoint(channel_def, &ip_endpoint)) {
      return ip_endpoint.ToString();
    }
  }

  return ::base::EmptyString();
}

void FillChannelDef(ChannelDef* channel_def) {
  IPEndPoint* ip_endpoint = channel_def->mutable_ip_endpoint();
  ip_endpoint->set_ip(
      net::HostIPAddress(net::HOST_IP_ONLY_ALLOW_IPV4).ToString());
  ip_endpoint->set_port(
      net::PickRandomPort(channel_def->type() == ChannelDef::TCP));
}

bool IsValidChannelDef(const ChannelDef& channel_def) {
  ::net::IPEndPoint ip_endpoint;
  return ToNetIPEndPoint(channel_def, &ip_endpoint);
}

bool IsValidChannelSource(const ChannelSource& channel_source) {
  if (channel_source.channel_defs_size() == 0) return false;

  for (auto& channel_def : channel_source.channel_defs()) {
    if (!IsValidChannelDef(channel_def)) return false;
  }
  return true;
}

bool IsSameChannelDef(const ChannelDef& c, const ChannelDef& c2) {
  ::net::IPEndPoint ip_endpoint;
  if (!ToNetIPEndPoint(c, &ip_endpoint)) return false;
  ::net::IPEndPoint ip_endpoint2;
  if (!ToNetIPEndPoint(c2, &ip_endpoint2)) return false;

  return ip_endpoint == ip_endpoint2;
}

EXPORT bool IsSameChannelSource(const ChannelSource& c,
                                const ChannelSource& c2) {
  if (c.channel_defs_size() != c2.channel_defs_size()) return false;

  for (int i = 0; i < c.channel_defs_size(); ++i) {
    if (!IsSameChannelDef(c.channel_defs(i), c2.channel_defs(i))) return false;
  }
  return true;
}

}  // namespace felicia