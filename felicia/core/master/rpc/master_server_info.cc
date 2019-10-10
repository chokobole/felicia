#include "felicia/core/master/rpc/master_server_info.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"

#include "felicia/core/lib/net/net_util.h"

namespace felicia {

static const uint16_t g_default_master_port = 8881;

net::IPAddress ResolveMasterServerIp() {
  const char* ip_env = getenv("FEL_MASTER_SERVER_IP");
  if (ip_env) {
    net::IPAddress address;
    bool ret = address.AssignFromIPLiteral(ip_env);
    CHECK(ret);
    return address;
  }

  return HostIPAddress(HOST_IP_ONLY_ALLOW_IPV4);
}

uint16_t ResolveMasterServerPort() {
  const char* port_env = getenv("FEL_MASTER_SERVER_PORT");
  if (port_env) {
    int value;
    if (base::StringToInt(port_env, &value)) {
      if (1023 < value && value <= 65535) {
        return value;
      }
    }
  }

  return g_default_master_port;
}

}  // namespace felicia