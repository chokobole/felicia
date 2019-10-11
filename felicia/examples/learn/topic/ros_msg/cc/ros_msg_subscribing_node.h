#ifndef FELICIA_EXAMPLES_LEARN_TOPIC_ROS_MSG_CC_ROS_MSG_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_TOPIC_ROS_MSG_CC_ROS_MSG_SUBSCRIBING_NODE_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/examples/learn/topic/common/cc/topic_create_flag.h"
#include "felicia/examples/learn/topic/ros_msg/SimpleMessage.h"

namespace felicia {

class RosMsgSubscribingNode : public NodeLifecycle {
 public:
  explicit RosMsgSubscribingNode(const TopicCreateFlag& topic_create_flag);

  // NodeLifecycle methods
  void OnDidCreate(const NodeInfo& node_info) override;

 private:
  void OnMessage(SimpleMessage&& message);

  void RequestSubscribe();

  NodeInfo node_info_;
  const TopicCreateFlag& topic_create_flag_;
  const std::string topic_;
  Subscriber<SimpleMessage> subscriber_;
};
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_TOPIC_ROS_MSG_CC_ROS_MSG_SUBSCRIBING_NODE_H_