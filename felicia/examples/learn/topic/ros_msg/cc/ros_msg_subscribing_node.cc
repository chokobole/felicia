#include "felicia/examples/learn/topic/ros_msg/cc/ros_msg_subscribing_node.h"

#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/master/master_proxy.h"

namespace felicia {

RosMsgSubscribingNode::RosMsgSubscribingNode(
    const TopicCreateFlag& topic_create_flag)
    : topic_create_flag_(topic_create_flag),
      topic_(topic_create_flag_.topic_flag()->value()) {}

void RosMsgSubscribingNode::OnDidCreate(NodeInfo node_info) {
  node_info_ = std::move(node_info);
  RequestSubscribe();
}

void RosMsgSubscribingNode::OnMessage(SimpleMessage&& message) {
  std::cout << "message : " << message << std::endl;
}

void RosMsgSubscribingNode::RequestSubscribe() {
  communication::Settings settings;
  settings.buffer_size = Bytes::FromBytes(512);

  subscriber_.RequestSubscribe(
      node_info_, topic_,
      ChannelDef::CHANNEL_TYPE_TCP | ChannelDef::CHANNEL_TYPE_UDP |
          ChannelDef::CHANNEL_TYPE_UDS | ChannelDef::CHANNEL_TYPE_SHM,
      settings,
      base::BindRepeating(&RosMsgSubscribingNode::OnMessage,
                          base::Unretained(this)));
}

}  // namespace felicia