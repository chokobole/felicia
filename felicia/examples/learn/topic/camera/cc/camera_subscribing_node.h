#ifndef FELICIA_EXAMPLES_LEARN_TOPIC_CAMERA_CC_CAMERA_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_TOPIC_CAMERA_CC_CAMERA_SUBSCRIBING_NODE_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/camera_frame_message.pb.h"
#include "felicia/examples/learn/topic/common/cc/camera_flag.h"

namespace felicia {

class CameraSubscribingNode : public NodeLifecycle {
 public:
  explicit CameraSubscribingNode(const CameraFlag& camera_flag);

  // NodeLifecycle methods
  void OnDidCreate(const NodeInfo& node_info) override;

 private:
  void OnMessage(drivers::CameraFrameMessage&& message);

  void RequestSubscribe();

  NodeInfo node_info_;
  const CameraFlag& camera_flag_;
  const std::string topic_;
  Subscriber<drivers::CameraFrameMessage> subscriber_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_TOPIC_CAMERA_CC_CAMERA_SUBSCRIBING_NODE_H_