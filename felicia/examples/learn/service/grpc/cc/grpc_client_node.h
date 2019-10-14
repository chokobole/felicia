#ifndef FELICIA_EXAMPLES_LEARN_SERVICE_GRPC_CC_GRPC_CLIENT_NODE_H_
#define FELICIA_EXAMPLES_LEARN_SERVICE_GRPC_CC_GRPC_CLIENT_NODE_H_

#include "felicia/core/communication/service_client.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/examples/learn/service/grpc/cc/grpc_service_flag.h"
#include "felicia/examples/learn/service/grpc/simple_service.grpc.pb.h"

namespace felicia {

class SimpleClient : public rpc::Client<grpc::SimpleService> {
 public:
  FEL_CLIENT_METHOD_DECLARE(Add);
};

class GrpcClientNode : public NodeLifecycle {
 public:
  explicit GrpcClientNode(const GrpcServiceFlag& grpc_service_flag);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(const NodeInfo& node_info) override;
  void OnError(const Status& s) override;

 private:
  void OnRequestRegister(const Status& s);
  void OnRequestUnegister(const Status& s);
  void OnConnect(ServiceInfo::Status s);
  void OnRequestAdd(const AddRequest* request, AddResponse* response,
                    const Status& s);

  void RequestRegister();
  void RequestUnregister();
  void RequestAdd();

  NodeInfo node_info_;
  const GrpcServiceFlag& grpc_service_flag_;
  const std::string service_;
  ServiceClient<SimpleClient> client_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_SERVICE_GRPC_CC_GRPC_CLIENT_NODE_H_