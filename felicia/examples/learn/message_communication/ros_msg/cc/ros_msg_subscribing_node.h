#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_ROS_MSG_CC_ROS_MSG_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_ROS_MSG_CC_ROS_MSG_SUBSCRIBING_NODE_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/examples/learn/message_communication/common/cc/node_create_flag.h"
#include "felicia/examples/learn/message_communication/ros_msg/SimpleMessage.h"

namespace felicia {

class RosMsgSubscribingNode : public NodeLifecycle {
 public:
  explicit RosMsgSubscribingNode(const NodeCreateFlag& node_create_flag);

  // NodeLifecycle methods
  void OnDidCreate(const NodeInfo& node_info) override;

 private:
  void OnMessage(SimpleMessage&& message);

  void RequestSubscribe();

  NodeInfo node_info_;
  const NodeCreateFlag& node_create_flag_;
  const std::string topic_;
  Subscriber<SimpleMessage> subscriber_;
};
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_ROS_MSG_CC_ROS_MSG_SUBSCRIBING_NODE_H_