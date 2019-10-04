#include "felicia/examples/learn/message_communication/ros_msg/cc/ros_msg_publishing_node.h"

#include "felicia/core/master/master_proxy.h"

namespace felicia {

RosMsgPublishingNode::RosMsgPublishingNode(
    const NodeCreateFlag& node_create_flag)
    : node_create_flag_(node_create_flag),
      topic_(node_create_flag_.topic_flag()->value()) {}

void RosMsgPublishingNode::OnDidCreate(const NodeInfo& node_info) {
  node_info_ = node_info;
  RequestPublish();
}

void RosMsgPublishingNode::OnRequestPublish(const Status& s) {
  LOG_IF(ERROR, !s.ok()) << s;
  RepeatingPublish();
}

void RosMsgPublishingNode::RequestPublish() {
  communication::Settings settings;
  settings.buffer_size = Bytes::FromBytes(512);

  ChannelDef::Type channel_type;
  ChannelDef::Type_Parse(node_create_flag_.channel_type_flag()->value(),
                         &channel_type);

  publisher_.RequestPublish(
      node_info_, topic_, channel_type, settings,
      base::BindOnce(&RosMsgPublishingNode::OnRequestPublish,
                     base::Unretained(this)));
}

void RosMsgPublishingNode::RepeatingPublish() {
  publisher_.Publish(GenerateMessage());

  if (!publisher_.IsUnregistered()) {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&RosMsgPublishingNode::RepeatingPublish,
                       base::Unretained(this)),
        base::TimeDelta::FromSeconds(1));
  }
}

SimpleMessage RosMsgPublishingNode::GenerateMessage() {
  static int id = 0;
  base::TimeDelta timestamp = timestamper_.timestamp();
  SimpleMessage message;
  message.id = id++;
  message.timestamp = timestamp.InMicroseconds();
  message.content = "hello world";
  return message;
}

}  // namespace felicia