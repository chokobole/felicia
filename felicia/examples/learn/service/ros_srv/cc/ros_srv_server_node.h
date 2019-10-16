#ifndef FELICIA_EXAMPLES_LEARN_SERVICE_ROS_SRV_CC_ROS_SRV_SERVER_NODE_H_
#define FELICIA_EXAMPLES_LEARN_SERVICE_ROS_SRV_CC_ROS_SRV_SERVER_NODE_H_

#include "felicia/core/communication/service_server.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/rpc/service.h"
#include "felicia/examples/learn/service/common/cc/simple_service_flag.h"
#include "felicia/examples/learn/service/ros_srv/SimpleService.h"

namespace felicia {

class RosSrvSimpleService : public rpc::Service<SimpleService> {
 public:
  void Handle(const SimpleServiceRequest* request,
              SimpleServiceResponse* response,
              StatusOnceCallback callback) override;
};

class RosSrvServerNode : public NodeLifecycle {
 public:
  explicit RosSrvServerNode(const SimpleServiceFlag& simple_service_flag);

  // NodeLifecycle methods
  void OnDidCreate(const NodeInfo& node_info) override;

 private:
  void RequestRegister();
  void RequestUnregister();

  NodeInfo node_info_;
  const std::string service_;
  ServiceServer<RosSrvSimpleService> server_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_SERVICE_GRPC_CC_ROS_SRV_SERVER_NODE_H_