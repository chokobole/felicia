#ifndef FELICIA_EXAMPLES_LEARN_TOPIC_ROS_MSG_CC_ROS_MSG_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_TOPIC_ROS_MSG_CC_ROS_MSG_PUBLISHING_NODE_H_

#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/util/timestamp/timestamper.h"
#include "felicia/examples/learn/topic/common/cc/topic_create_flag.h"
#include "felicia/examples/learn/topic/ros_msg/SimpleMessage.h"

namespace felicia {

class RosMsgPublishingNode : public NodeLifecycle {
 public:
  RosMsgPublishingNode(const TopicCreateFlag& topic_create_flag);

  // NodeLifecycle methods
  void OnDidCreate(NodeInfo node_info) override;

 private:
  void OnRequestPublish(Status s);

  void RequestPublish();

  void RepeatingPublish();
  SimpleMessage GenerateMessage();

  NodeInfo node_info_;
  const TopicCreateFlag& topic_create_flag_;
  const std::string topic_;
  Publisher<SimpleMessage> publisher_;
  Timestamper timestamper_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_TOPIC_ROS_MSG_CC_ROS_MSG_PUBLISHING_NODE_H_