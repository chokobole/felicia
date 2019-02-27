#include "felicia/core/platform/net_util.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/rand_util.h"
#include "third_party/chromium/net/base/ip_endpoint.h"
#include "third_party/chromium/net/base/net_errors.h"
#include "third_party/chromium/net/base/network_interfaces.h"
#include "third_party/chromium/net/socket/tcp_socket.h"
#include "third_party/chromium/net/socket/udp_socket.h"

namespace felicia {
namespace net {

// Returns the host ip address of the machine on which this process is running
EXPORT std::string HostIPAddress(int option) {
  ::net::NetworkInterfaceList list;
  ::net::GetNetworkList(&list, ::net::EXCLUDE_HOST_SCOPE_VIRTUAL_INTERFACES);
  std::string text;
  for (auto it = list.begin(); it != list.end(); ++it) {
    if (it->type == ::net::NetworkChangeNotifier::CONNECTION_ETHERNET) {
      if (HOST_IP_ONLY_ALLOW_IPV4 & option) {
        if (it->address.IsIPv4()) {
          text = it->address.ToString();
          break;
        }
        continue;
      }
      text = it->address.ToString();
      break;
    }
  }
  return text;
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

    rv = tcp_socket.SetDefaultOptionsForServer();
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