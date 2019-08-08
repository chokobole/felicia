#include <stdint.h>

#include "third_party/chromium/net/base/ip_address.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

EXPORT net::IPAddress ResolveGRPCServiceIp();

EXPORT uint16_t ResolveGRPCServicePort();

}  // namespace felicia
