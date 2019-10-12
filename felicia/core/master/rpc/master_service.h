#ifndef FELICIA_CORE_MASTER_RPC_MASTER_SERVICE_H_
#define FELICIA_CORE_MASTER_RPC_MASTER_SERVICE_H_

#include "felicia/core/master/master.h"
#include "felicia/core/master/rpc/master_service.grpc.pb.h"
#include "felicia/core/rpc/service.h"

namespace felicia {

class MasterService : public rpc::Service<grpc::MasterService> {
 public:
  MasterService(Master* master, ::grpc::ServerBuilder* builder);

 private:
  void EnqueueRequests() override;

#define MASTER_METHOD(Method, method, cancelable) \
  FEL_SERVICE_METHOD_DECLARE(MasterService, Method);
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD

  Master* master_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_RPC_GRPC_MASTER_SERVICE_H_