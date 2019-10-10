#include "felicia/core/rpc/grpc_util.h"

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/stringprintf.h"

namespace felicia {

std::shared_ptr<::grpc::Channel> ConnectToGrpcServer(const std::string& ip,
                                                     uint16_t port) {
  auto channel =
      ::grpc::CreateChannel(base::StringPrintf("%s:%d", ip.c_str(), port),
                            ::grpc::InsecureChannelCredentials());
  if (!channel->WaitForConnected(
          gpr_time_add(gpr_now(GPR_CLOCK_REALTIME),
                       gpr_time_from_seconds(10, GPR_TIMESPAN)))) {
    LOG(ERROR) << "Channel to server failed to connected";
    NOTREACHED();
    return nullptr;
  }
  LOG(INFO) << "Channel to server is connected on port " << port;

  return channel;
}

}  // namespace felicia