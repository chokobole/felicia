#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_ROS_MSG_CC_ROS_MSG_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_ROS_MSG_CC_ROS_MSG_PUBLISHING_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/util/timestamp/timestamper.h"
#include "felicia/examples/learn/message_communication/common/cc/node_create_flag.h"
#include "felicia/examples/learn/message_communication/ros_msg/SimpleMessage.h"

namespace felicia {

class RosMsgPublishingNode : public NodeLifecycle {
 public:
  RosMsgPublishingNode(const NodeCreateFlag& node_create_flag);

  // NodeLifecycle methods
  void OnDidCreate(const NodeInfo& node_info) override;

 private:
  void OnRequestPublish(const Status& s);

  void RequestPublish();

  void RepeatingPublish();
  SimpleMessage GenerateMessage();

  NodeInfo node_info_;
  const NodeCreateFlag& node_create_flag_;
  const std::string topic_;
  Publisher<SimpleMessage> publisher_;
  Timestamper timestamper_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_ROS_MSG_CC_ROS_MSG_PUBLISHING_NODE_H_