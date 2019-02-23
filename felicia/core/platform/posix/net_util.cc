#include "felicia/core/platform/net_util.h"

#include <ifaddrs.h>
#include <net/if.h>
#include <unistd.h>

#include <unordered_set>

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/rand_util.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/net/base/ip_endpoint.h"
#include "third_party/chromium/net/base/net_errors.h"
#include "third_party/chromium/net/socket/tcp_socket.h"
#include "third_party/chromium/net/socket/udp_socket.h"

namespace felicia {
namespace net {

std::string Hostname() {
  char hostname[1024];
  gethostname(hostname, sizeof hostname);
  hostname[sizeof hostname - 1] = 0;
  return std::string(hostname);
}

std::string HostIPAddress(int option) {
  struct ifaddrs* addrs;
  int rv = getifaddrs(&addrs);
  if (rv < 0) {
    LOG(WARNING) << "getifaddrs failed " << rv;
    return "";
  }

  ::net::IPEndPoint src;
  for (struct ifaddrs* ifa = addrs; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr->sa_family == AF_INET) {
      if (!src.FromSockAddr(ifa->ifa_addr, sizeof(struct sockaddr_in)))
        continue;
    } else if (ifa->ifa_addr->sa_family == AF_INET6) {
      if (!src.FromSockAddr(ifa->ifa_addr, sizeof(struct sockaddr_in6)))
        continue;
    }

    if (!(IFF_UP & ifa->ifa_flags)) continue;

    if (IFF_LOOPBACK & ifa->ifa_flags) {
      if (HOST_IP_ALLOW_LOOPBACK & option) break;
    }

    if (::base::StartsWith(ifa->ifa_name, "en",
                           ::base::CompareCase::SENSITIVE)) {
      if (HOST_IP_ONLY_ALLOW_IPV4 & option) {
        if (ifa->ifa_addr->sa_family == AF_INET) break;
        continue;
      }
      break;
    }
  }

  freeifaddrs(addrs);
  return src.address().ToString();
}

uint32_t MulticastInterfaceIndex(int option) {
  uint32_t index = 0;
  struct ifaddrs* addrs;
  int rv = getifaddrs(&addrs);
  if (rv < 0) {
    LOG(WARNING) << "getifaddrs failed " << rv;
    return index;
  }

  ::net::IPEndPoint src;
  for (struct ifaddrs* ifa = addrs; ifa != NULL; ifa = ifa->ifa_next) {
    if (!(IFF_UP & ifa->ifa_flags)) {
      index++;
      continue;
    }

    if (IFF_MULTICAST & ifa->ifa_flags) {
      if (HOST_IP_ONLY_ALLOW_IPV4 & option) {
        if (ifa->ifa_addr->sa_family == AF_INET) {
          return index;
        }
      }
    }

    index++;
  }

  freeifaddrs(addrs);
  return index;
}

namespace {

bool IsPortAvailable(uint16_t* port, bool is_tcp) {
  ::net::IPAddress address(0, 0, 0, 0);
  ::net::IPEndPoint endpoint(address, *port);
  if (is_tcp) {
    ::net::TCPSocket tcp_socket(nullptr);
    int rv = tcp_socket.Open(::net::ADDRESS_FAMILY_IPV4);
    if (rv != ::net::OK) return false;

    rv = tcp_socket.Bind(endpoint);
    if (rv != ::net::OK) return false;

    rv = tcp_socket.AllowAddressReuse();
    if (rv != ::net::OK) return false;

    rv = tcp_socket.GetLocalAddress(&endpoint);
    if (rv != 0) return false;
  } else {
    ::net::UDPSocket udp_socket(::net::DatagramSocket::DEFAULT_BIND);
    int rv = udp_socket.Open(::net::ADDRESS_FAMILY_IPV4);
    if (rv != ::net::OK) return false;

    // UDP socket should |AllowAddressReuse| before |Bind|.
    rv = udp_socket.AllowAddressReuse();
    if (rv != ::net::OK) return false;

    rv = udp_socket.Bind(endpoint);
    if (rv != ::net::OK) return false;

    rv = udp_socket.GetLocalAddress(&endpoint);
    if (rv != 0) return false;
  }

  if (*port == 0) {
    *port = endpoint.port();
  } else {
    CHECK_EQ(*port, endpoint.port());
  }
  return true;
}

}  // namespace

uint16_t PickRandomPort(bool is_tcp) {
  int trial = 0;
  while (true) {
    uint16_t port = 0;
    if (trial < 100) {
      port = ::base::RandInt(1024, 65535);
      trial++;
    }

    if (!IsPortAvailable(&port, is_tcp)) continue;

    CHECK_GT(port, 0);
    return port;
  }

  return 0;
}

}  // namespace net
}  // namespace felicia