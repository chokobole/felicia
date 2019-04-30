#include "felicia/core/channel/channel.h"

#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/lib/net/net_util.h"

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
                              ChannelDef_Type type) {
  ChannelSource channel_source;
  IPEndPoint* endpoint = channel_source.mutable_ip_endpoint();
  endpoint->set_ip(ip_endpoint.address().ToString());
  endpoint->set_port(ip_endpoint.port());
  ChannelDef channel_def;
  channel_def.set_type(type);
  *channel_source.mutable_channel_def() = channel_def;
  return channel_source;
}

std::string ChannelDefToString(const ChannelDef& channel_def) {
  if (channel_def.type() == ChannelDef::TCP)
    return "TCP";
  else if (channel_def.type() == ChannelDef::UDP)
    return "UDP";
  NOTREACHED();
}

ChannelDef ChannelDefFromString(const std::string& str) {
  ChannelDef channel_def;
  if (str.compare("TCP") == 0)
    channel_def.set_type(ChannelDef::TCP);
  else if (str.compare("UDP") == 0)
    channel_def.set_type(ChannelDef::UDP);
  else
    LOG(WARNING) << "Unknown String: " << str;
  return channel_def;
}

std::string ChannelSourceToString(const ChannelSource& channel_source) {
  if (channel_source.has_ip_endpoint()) {
    ::net::IPEndPoint ip_endpoint;
    if (ToNetIPEndPoint(channel_source, &ip_endpoint)) {
      return ip_endpoint.ToString();
    }
  }
  return ::base::EmptyString();
}

ChannelSource PickRandomChannelSource(ChannelDef_Type type) {
  ChannelSource channel_source;
  IPEndPoint* ip_endpoint = channel_source.mutable_ip_endpoint();
  ip_endpoint->set_ip(
      net::HostIPAddress(net::HOST_IP_ONLY_ALLOW_IPV4).ToString());
  ip_endpoint->set_port(net::PickRandomPort(type == ChannelDef::TCP));
  ChannelDef channel_def;
  channel_def.set_type(type);
  *channel_source.mutable_channel_def() = channel_def;
  return channel_source;
}

bool IsValidChannelSource(const ChannelSource& channel_source) {
  ::net::IPEndPoint ip_endpoint;
  return ToNetIPEndPoint(channel_source, &ip_endpoint);
}

bool IsSameChannelSource(const ChannelSource& c, const ChannelSource& c2) {
  ::net::IPEndPoint ip_endpoint;
  if (!ToNetIPEndPoint(c, &ip_endpoint)) return false;
  ::net::IPEndPoint ip_endpoint2;
  if (!ToNetIPEndPoint(c2, &ip_endpoint2)) return false;

  return ip_endpoint == ip_endpoint2;
}

}  // namespace felicia