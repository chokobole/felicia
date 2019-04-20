#include "felicia/core/master/rpc/grpc_util.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/master/rpc/grpc_info.h"

namespace felicia {

std::shared_ptr<::grpc::Channel> ConnectGRPCService() {
  std::string ip = ResolveGRPCServiceIp().ToString();
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