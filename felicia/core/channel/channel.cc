#include "felicia/core/channel/channel.h"

#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/net/net_util.h"

namespace felicia {

Status ToNetIPEndPoint(const ChannelDef& channel_def,
                       ::net::IPEndPoint* ip_endpoint) {
  if (!channel_def.has_ip_endpoint()) {
    return errors::InvalidArgument(
        "channel_def doesn't contain an IPEndPoint.");
  }

  ::net::IPAddress ip;
  if (!ip.AssignFromIPLiteral(channel_def.ip_endpoint().ip())) {
    return errors::InvalidArgument("Failed to convert to IPAddress.");
  }

  *ip_endpoint = ::net::IPEndPoint(ip, channel_def.ip_endpoint().port());
  return Status::OK();
}

#if defined(OS_POSIX)
Status ToNetUDSEndPoint(const ChannelDef& channel_def,
                        ::net::UDSEndPoint* uds_endpoint) {
  if (!channel_def.has_uds_endpoint()) {
    return errors::InvalidArgument(
        "channel_def doesn't contain a UDSEndPoint.");
  }

  UDSEndPoint endpoint = channel_def.uds_endpoint();
  uds_endpoint->set_socket_path(endpoint.socket_path());
  uds_endpoint->set_use_abstract_namespace(endpoint.use_abstract_namespace());

  ::net::SockaddrStorage address;
  if (!uds_endpoint->ToSockAddrStorage(&address)) {
    return errors::InvalidArgument("Failed to convert to SockAddrStorage.");
  }

  return Status::OK();
}
#endif

std::string EndPointToString(const ChannelDef& channel_def) {
  if (channel_def.has_ip_endpoint()) {
    ::net::IPEndPoint ip_endpoint;
    if (ToNetIPEndPoint(channel_def, &ip_endpoint).ok()) {
      return ip_endpoint.ToString();
    }
  } else if (channel_def.has_uds_endpoint()) {
#if defined(OS_POSIX)
    ::net::UDSEndPoint uds_endpoint;
    if (ToNetUDSEndPoint(channel_def, &uds_endpoint).ok()) {
      return uds_endpoint.ToString();
    }
#endif
  } else if (channel_def.has_shm_endpoint()) {
    return channel_def.shm_endpoint().DebugString();
  }

  return ::base::EmptyString();
}

bool IsValidChannelDef(const ChannelDef& channel_def) {
  ChannelDef::Type type = channel_def.type();
  if (type == ChannelDef::CHANNEL_TYPE_TCP ||
      type == ChannelDef::CHANNEL_TYPE_UDP ||
      type == ChannelDef::CHANNEL_TYPE_WS) {
    ::net::IPEndPoint ip_endpoint;
    return ToNetIPEndPoint(channel_def, &ip_endpoint).ok();
  } else if (type == ChannelDef::CHANNEL_TYPE_UDS) {
#if defined(OS_POSIX)
    ::net::UDSEndPoint uds_endpoint;
    return ToNetUDSEndPoint(channel_def, &uds_endpoint).ok();
#endif
  } else if (type == ChannelDef::CHANNEL_TYPE_SHM) {
    return true;
  }
  return false;
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
  if (!ToNetIPEndPoint(c, &ip_endpoint).ok()) return false;
  ::net::IPEndPoint ip_endpoint2;
  if (!ToNetIPEndPoint(c2, &ip_endpoint2).ok()) return false;

  return ip_endpoint == ip_endpoint2;
}

bool IsSameChannelSource(const ChannelSource& c, const ChannelSource& c2) {
  if (c.channel_defs_size() != c2.channel_defs_size()) return false;

  for (int i = 0; i < c.channel_defs_size(); ++i) {
    if (!IsSameChannelDef(c.channel_defs(i), c2.channel_defs(i))) return false;
  }
  return true;
}

}  // namespace felicia