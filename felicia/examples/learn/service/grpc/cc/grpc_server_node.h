#ifndef FELICIA_EXAMPLES_LEARN_SERVICE_GRPC_CC_GRPC_SERVER_NODE_H_
#define FELICIA_EXAMPLES_LEARN_SERVICE_GRPC_CC_GRPC_SERVER_NODE_H_

#include "felicia/core/communication/service_server.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/rpc/service.h"
#include "felicia/examples/learn/service/grpc/cc/grpc_service_flag.h"
#include "felicia/examples/learn/service/grpc/simple_service.grpc.pb.h"

namespace felicia {

class SimpleService : public rpc::Service<grpc::SimpleService> {
 public:
  explicit SimpleService(::grpc::ServerBuilder* builder);

 private:
  void EnqueueRequests() override;

  FEL_SERVICE_METHOD_DECLARE(SimpleService, Add);

  void Add(const AddRequest* request, AddResponse* response,
           StatusOnceCallback callback);
};

class GrpcServerNode : public NodeLifecycle {
 public:
  explicit GrpcServerNode(const GrpcServiceFlag& grpc_service_flag);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(const NodeInfo& node_info) override;
  void OnError(const Status& s) override;

 private:
  void OnRequestRegister(const Status& s);
  void OnRequestUnregister(const Status& s);

  void RequestRegister();
  void RequestUnregister();

  NodeInfo node_info_;
  const std::string service_;
  ServiceServer<SimpleService> server_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_SERVICE_GRPC_CC_GRPC_SERVER_NODE_H_