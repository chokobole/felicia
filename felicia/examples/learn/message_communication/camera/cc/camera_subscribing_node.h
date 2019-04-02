#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_SUBSCRIBING_NODE_H_

#include "felicia/core/node/node_lifecycle.h"
#include "felicia/drivers/camera/camera_factory.h"

namespace felicia {

class CameraSubscribingNode : public NodeLifecycle {
 public:
  CameraSubscribingNode(const std::string& topic) : topic_(topic) {}

  void OnInit() override {
    std::cout << "CameraSubscribingNode::OnInit()" << std::endl;
  }

 private:
  NodeInfo node_info_;
  std::string topic_;
  ChannelDef channel_def_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_SUBSCRIBING_NODE_H_