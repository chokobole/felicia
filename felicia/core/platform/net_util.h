#ifndef FELICIA_CORE_PLATFORM_NET_UTIL_H_
#define FELICIA_CORE_PLATFORM_NET_UTIL_H_

#include <string>

#include "felicia/core/lib/base/export.h"

namespace felicia {
namespace net {

// Return the hostname of the machine on which this process is running
EXPORT std::string Hostname();

const int HOST_IP_ONLY_ALLOW_IPV4 = 1;
const int HOST_IP_ALLOW_LOOPBACK = 2;

// Returns the host ip address of the machine on which this process is running
EXPORT std::string HostIPAddress(int option = 0);

// Returns the first interface index which supports multicast
EXPORT uint32_t MulticastInterfaceIndex(int option = 0);

// Retunrs the randomly picked port
EXPORT uint16_t PickRandomPort(bool is_tcp);

}  // namespace net
}  // namespace felicia

#endif  // FELICIA_CORE_PLATFORM_NET_UTIL_H_
