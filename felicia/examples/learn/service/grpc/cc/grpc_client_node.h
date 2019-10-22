#ifndef FELICIA_EXAMPLES_LEARN_SERVICE_GRPC_CC_GRPC_CLIENT_NODE_H_
#define FELICIA_EXAMPLES_LEARN_SERVICE_GRPC_CC_GRPC_CLIENT_NODE_H_

#include "felicia/core/communication/service_client.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/rpc/client.h"
#include "felicia/examples/learn/service/common/cc/simple_service_flag.h"
#include "felicia/examples/learn/service/grpc/simple_service.grpc.pb.h"

namespace felicia {

class GrpcSimpleClient : public rpc::Client<grpc::SimpleService> {
 public:
  FEL_GRPC_CLIENT_METHOD_DECLARE(Add);
};

class GrpcClientNode : public NodeLifecycle {
 public:
  explicit GrpcClientNode(const SimpleServiceFlag& simple_service_flag);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(const NodeInfo& node_info) override;
  void OnError(Status s) override;

 private:
  void OnRequestRegister(Status s);
  void OnRequestUnegister(Status s);
  void OnConnect(ServiceInfo::Status s);
  void OnRequestAdd(const AddRequest* request, const AddResponse* response,
                    Status s);

  void RequestRegister();
  void RequestUnregister();
  void RequestAdd();

  NodeInfo node_info_;
  const SimpleServiceFlag& simple_service_flag_;
  const std::string service_;
  ServiceClient<GrpcSimpleClient> client_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_SERVICE_GRPC_CC_GRPC_CLIENT_NODE_H_