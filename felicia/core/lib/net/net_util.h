#ifndef FELICIA_CORE_LIB_NET_NET_UTIL_H_
#define FELICIA_CORE_LIB_NET_NET_UTIL_H_

#include "third_party/chromium/net/base/ip_address.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {
namespace net {

const int HOST_IP_ONLY_ALLOW_IPV4 = 1;

// Returns the host ip address of the machine on which this process is running
EXPORT ::net::IPAddress HostIPAddress(int option = 0);

// Retunrs the randomly picked port
EXPORT uint16_t PickRandomPort(bool is_tcp);

}  // namespace net
}  // namespace felicia

#endif  // FELICIA_CORE_LIB_NET_NET_UTIL_H_
