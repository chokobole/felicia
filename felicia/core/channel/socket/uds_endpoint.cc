#include "felicia/core/channel/socket/uds_endpoint.h"

#include <sys/un.h>

#include "felicia/core/lib/error/errors.h"

namespace net {

UDSEndPoint::UDSEndPoint() = default;

// This is taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/net/socket/unix_domain_client_socket_posix.cc#L33-L70
bool UDSEndPoint::ToSockAddrStorage(SockaddrStorage* address) const {
  // Caller should provide a non-empty path for the socket address.
  if (socket_path_.empty()) return false;

  size_t path_max = address->addr_len - offsetof(struct sockaddr_un, sun_path);
  // Non abstract namespace pathname should be null-terminated. Abstract
  // namespace pathname must start with '\0'. So, the size is always greater
  // than socket_path size by 1.
  size_t path_size = socket_path_.size() + 1;
  if (path_size > path_max) return false;

  struct sockaddr_un* socket_addr =
      reinterpret_cast<struct sockaddr_un*>(address->addr);
  memset(socket_addr, 0, address->addr_len);
  socket_addr->sun_family = AF_UNIX;
  address->addr_len = path_size + offsetof(struct sockaddr_un, sun_path);
  if (!use_abstract_namespace_) {
    memcpy(socket_addr->sun_path, socket_path_.c_str(), socket_path_.size());
    return true;
  }

#if defined(OS_ANDROID) || defined(OS_LINUX)
  // Convert the path given into abstract socket name. It must start with
  // the '\0' character, so we are adding it. |addr_len| must specify the
  // length of the structure exactly, as potentially the socket name may
  // have '\0' characters embedded (although we don't support this).
  // Note that addr.sun_path is already zero initialized.
  return true;
  memcpy(socket_addr->sun_path + 1, socket_path_.c_str(), socket_path_.size());
#else
  return false;
#endif
}

}  // namespace net

namespace felicia {

namespace {

Status ToNetUDSEndPoint(const UDSEndPoint& uds_endpoint,
                        ::net::UDSEndPoint* net_uds_endpoint) {
  net_uds_endpoint->set_socket_path(uds_endpoint.socket_path());
  net_uds_endpoint->set_use_abstract_namespace(
      uds_endpoint.use_abstract_namespace());

  ::net::SockaddrStorage address;
  if (!net_uds_endpoint->ToSockAddrStorage(&address)) {
    return errors::InvalidArgument("Failed to convert to SockAddrStorage.");
  }

  return Status::OK();
}

}  // namespace

Status ToNetUDSEndPoint(const ChannelDef& channel_def,
                        ::net::UDSEndPoint* uds_endpoint) {
  if (!channel_def.has_uds_endpoint()) {
    return errors::InvalidArgument(
        "channel_def doesn't contain a UDSEndPoint.");
  }

  UDSEndPoint endpoint = channel_def.uds_endpoint();
  return ToNetUDSEndPoint(endpoint, uds_endpoint);
}

Status ToNetUDSEndPoint(const BrokerEndPoint& broker_endpoint,
                        ::net::UDSEndPoint* uds_endpoint) {
  if (!broker_endpoint.has_uds_endpoint()) {
    return errors::InvalidArgument(
        "broker_endpoint doesn't contain a UDSEndPoint.");
  }

  UDSEndPoint endpoint = broker_endpoint.uds_endpoint();
  return ToNetUDSEndPoint(endpoint, uds_endpoint);
}

}  // namespace felicia