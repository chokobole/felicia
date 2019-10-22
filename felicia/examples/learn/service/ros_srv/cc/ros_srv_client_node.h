#ifndef FELICIA_EXAMPLES_LEARN_SERVICE_ROS_SRV_CC_ROS_SRV_CLIENT_NODE_H_
#define FELICIA_EXAMPLES_LEARN_SERVICE_ROS_SRV_CC_ROS_SRV_CLIENT_NODE_H_

#include "felicia/core/communication/service_client.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/rpc/client.h"
#include "felicia/examples/learn/service/common/cc/simple_service_flag.h"
#include "felicia/examples/learn/service/ros_srv/SimpleService.h"

namespace felicia {

class RosSrvClientNode : public NodeLifecycle {
 public:
  explicit RosSrvClientNode(const SimpleServiceFlag& simple_service_flag);

  // NodeLifecycle methods
  void OnDidCreate(const NodeInfo& node_info) override;

 private:
  void OnConnect(ServiceInfo::Status s);
  void OnRequestAdd(const SimpleServiceRequest* request,
                    SimpleServiceResponse* response, Status s);

  void RequestRegister();
  void RequestUnregister();
  void RequestAdd();

  NodeInfo node_info_;
  const SimpleServiceFlag& simple_service_flag_;
  const std::string service_;
  ServiceClient<rpc::Client<SimpleService>> client_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_SERVICE_ROS_SRV_CC_ROS_SRV_CLIENT_NODE_H_