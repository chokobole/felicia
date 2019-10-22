#include "felicia/examples/learn/topic/camera/cc/camera_subscribing_node.h"

namespace felicia {

CameraSubscribingNode::CameraSubscribingNode(const CameraFlag& camera_flag)
    : camera_flag_(camera_flag), topic_(camera_flag_.topic_flag()->value()) {}

void CameraSubscribingNode::OnDidCreate(NodeInfo node_info) {
  node_info_ = std::move(node_info);
  RequestSubscribe();
}

void CameraSubscribingNode::OnMessage(drivers::CameraFrameMessage&& message) {
  std::cout << "CameraSubscribingNode::OnMessage()" << std::endl;
}

void CameraSubscribingNode::RequestSubscribe() {
  communication::Settings settings;
  settings.queue_size = 1;
  settings.period =
      base::TimeDelta::FromSecondsD(1.0 / camera_flag_.fps_flag()->value());
  settings.is_dynamic_buffer = true;

  subscriber_.RequestSubscribe(
      node_info_, topic_,
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_SHM, settings,
      base::BindRepeating(&CameraSubscribingNode::OnMessage,
                          base::Unretained(this)));
}

}  // namespace felicia