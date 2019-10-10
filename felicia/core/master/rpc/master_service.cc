#include "felicia/core/master/rpc/master_service.h"

namespace felicia {

MasterService::MasterService(Master* master, ::grpc::ServerBuilder* builder)
    : Service<grpc::MasterService>(builder), master_(master) {}

void MasterService::EnqueueRequests() {
  FEL_ENQUEUE_REQUEST(MasterService, RegisterClient, true);
  FEL_ENQUEUE_REQUEST(MasterService, ListClients, false);
  FEL_ENQUEUE_REQUEST(MasterService, RegisterNode, true);
  FEL_ENQUEUE_REQUEST(MasterService, UnregisterNode, true);
  FEL_ENQUEUE_REQUEST(MasterService, ListNodes, false);
  FEL_ENQUEUE_REQUEST(MasterService, PublishTopic, true);
  FEL_ENQUEUE_REQUEST(MasterService, UnpublishTopic, true);
  FEL_ENQUEUE_REQUEST(MasterService, SubscribeTopic, true);
  FEL_ENQUEUE_REQUEST(MasterService, UnsubscribeTopic, true);
  FEL_ENQUEUE_REQUEST(MasterService, ListTopics, false);
}

FEL_SERVICE_METHOD_DEFINE(MasterService, master_, RegisterClient, true)
FEL_SERVICE_METHOD_DEFINE(MasterService, master_, ListClients, false)
FEL_SERVICE_METHOD_DEFINE(MasterService, master_, RegisterNode, true)
FEL_SERVICE_METHOD_DEFINE(MasterService, master_, UnregisterNode, true)
FEL_SERVICE_METHOD_DEFINE(MasterService, master_, ListNodes, false)
FEL_SERVICE_METHOD_DEFINE(MasterService, master_, PublishTopic, true)
FEL_SERVICE_METHOD_DEFINE(MasterService, master_, UnpublishTopic, true)
FEL_SERVICE_METHOD_DEFINE(MasterService, master_, SubscribeTopic, true)
FEL_SERVICE_METHOD_DEFINE(MasterService, master_, UnsubscribeTopic, true)
FEL_SERVICE_METHOD_DEFINE(MasterService, master_, ListTopics, false)

}  // namespace felicia