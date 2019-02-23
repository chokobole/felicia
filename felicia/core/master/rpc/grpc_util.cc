#include "felicia/core/master/rpc/grpc_util.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_number_conversions.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/platform/net_util.h"

namespace felicia {

static const uint16_t g_default_master_port = 8881;

std::string ResolveGRPCServiceIp() {
  const char* ip_env = getenv("FEL_GRPC_SERVICE_IP");
  if (ip_env) {
    return ip_env;
  }

  return felicia::net::HostIPAddress(net::HOST_IP_ONLY_ALLOW_IPV4);
}

uint16_t ResolveGRPCServicePort() {
  const char* port_env = getenv("FEL_GRPC_SERVICE_PORT");
  if (port_env) {
    int value;
    if (::base::StringToInt(port_env, &value)) {
      if (1023 < value && value <= 65535) {
        return value;
      }
    }
  }

  return g_default_master_port;
}

std::shared_ptr<::grpc::Channel> ConnectGRPCService() {
  std::string ip = ResolveGRPCServiceIp();
  uint16_t port = ResolveGRPCServicePort();
  auto channel =
      ::grpc::CreateChannel(::base::StringPrintf("%s:%d", ip.c_str(), port),
                            ::grpc::InsecureChannelCredentials());
  if (!channel->WaitForConnected(
          gpr_time_add(gpr_now(GPR_CLOCK_REALTIME),
                       gpr_time_from_seconds(10, GPR_TIMESPAN)))) {
    LOG(ERROR) << "Channel to server failed to connected";
    NOTREACHED();
  }
  LOG(INFO) << "Channel to server is connected on port " << port;

  return channel;
}

}  // namespace felicia