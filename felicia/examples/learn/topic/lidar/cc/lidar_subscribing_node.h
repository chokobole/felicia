#ifndef FELICIA_EXAMPLES_LEARN_TOPIC_LIDAR_CC_LIDAR_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_TOPIC_LIDAR_CC_LIDAR_SUBSCRIBING_NODE_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/lidar/lidar_frame_message.pb.h"
#include "felicia/examples/learn/topic/lidar/cc/lidar_flag.h"

namespace felicia {

class LidarSubscribingNode : public NodeLifecycle {
 public:
  explicit LidarSubscribingNode(const LidarFlag& lidar_flag);

  // NodeLifecycle methods
  void OnDidCreate(NodeInfo node_info) override;

 private:
  void OnMessage(drivers::LidarFrameMessage&& message);

  void RequestSubscribe();

  NodeInfo node_info_;
  const LidarFlag& lidar_flag_;
  const std::string topic_;
  Subscriber<drivers::LidarFrameMessage> subscriber_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_TOPIC_LIDAR_CC_LIDAR_SUBSCRIBING_NODE_H_