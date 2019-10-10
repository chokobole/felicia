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

  FEL_SERVICE_METHOD_DECLARE(MasterService, RegisterClient);
  FEL_SERVICE_METHOD_DECLARE(MasterService, ListClients);
  FEL_SERVICE_METHOD_DECLARE(MasterService, RegisterNode);
  FEL_SERVICE_METHOD_DECLARE(MasterService, UnregisterNode);
  FEL_SERVICE_METHOD_DECLARE(MasterService, ListNodes);
  FEL_SERVICE_METHOD_DECLARE(MasterService, PublishTopic);
  FEL_SERVICE_METHOD_DECLARE(MasterService, UnpublishTopic);
  FEL_SERVICE_METHOD_DECLARE(MasterService, SubscribeTopic);
  FEL_SERVICE_METHOD_DECLARE(MasterService, UnsubscribeTopic);
  FEL_SERVICE_METHOD_DECLARE(MasterService, ListTopics);

  Master* master_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_RPC_GRPC_MASTER_SERVICE_H_