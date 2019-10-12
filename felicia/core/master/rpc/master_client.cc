#include "felicia/core/master/rpc/master_client.h"

namespace felicia {

MasterClient::MasterClient(std::shared_ptr<::grpc::Channel> channel)
    : Client<grpc::MasterService>(channel) {}

MasterClient::~MasterClient() = default;

Status MasterClient::Start() { return Run(); }

Status MasterClient::Stop() { return Shutdown(); }

#define MASTER_METHOD(Method, method, cancelable) \
  FEL_CLIENT_METHOD_DEFINE(MasterClient, Method)
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD

}  // namespace felicia