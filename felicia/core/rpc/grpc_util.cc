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
    LOG(ERROR) << "Failed to connect to grpc server on port " << port;
    return nullptr;
  }

  return channel;
}

}  // namespace felicia