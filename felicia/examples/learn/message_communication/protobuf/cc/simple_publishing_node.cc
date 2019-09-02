#include "felicia/examples/learn/message_communication/protobuf/cc/simple_publishing_node.h"

#include "felicia/core/master/master_proxy.h"

namespace felicia {

SimplePublishingNode::SimplePublishingNode(
    const NodeCreateFlag& node_create_flag,
    SSLServerContext* ssl_server_context)
    : node_create_flag_(node_create_flag),
      topic_(node_create_flag_.topic_flag()->value()),
      ssl_server_context_(ssl_server_context) {}

void SimplePublishingNode::OnInit() {
  std::cout << "SimplePublishingNode::OnInit()" << std::endl;
}

void SimplePublishingNode::OnDidCreate(const NodeInfo& node_info) {
  std::cout << "SimplePublishingNode::OnDidCreate()" << std::endl;
  node_info_ = node_info;
  RequestPublish();

  // MasterProxy& master_proxy = MasterProxy::GetInstance();
  // master_proxy.PostDelayedTask(
  //     FROM_HERE,
  //     base::BindOnce(&SimplePublishingNode::RequestUnpublish,
  //                      base::Unretained(this)),
  //     base::TimeDelta::FromSeconds(10));
}

void SimplePublishingNode::OnError(const Status& s) {
  std::cout << "SimplePublishingNode::OnError()" << std::endl;
  LOG(ERROR) << s;
}

void SimplePublishingNode::OnPublish(ChannelDef::Type type, const Status& s) {
  std::cout << "SimplePublishingNode::OnPublish() from "
            << ChannelDef::Type_Name(type) << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

void SimplePublishingNode::OnRequestPublish(const Status& s) {
  std::cout << "SimplePublishingNode::OnRequestPublish()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
  RepeatingPublish();
}

void SimplePublishingNode::OnRequestUnpublish(const Status& s) {
  std::cout << "SimplePublishingNode::OnRequestUnpublish()" << std::endl;
  LOG_IF(ERROR, !s.ok()) << s;
}

void SimplePublishingNode::RequestPublish() {
  communication::Settings settings;
  settings.buffer_size = Bytes::FromBytes(512);
  if (ssl_server_context_) {
    settings.channel_settings.tcp_settings.use_ssl = true;
    settings.channel_settings.tcp_settings.ssl_server_context =
        ssl_server_context_;
  }

  ChannelDef::Type channel_type;
  ChannelDef::Type_Parse(node_create_flag_.channel_type_flag()->value(),
                         &channel_type);

  publisher_.RequestPublish(
      node_info_, topic_, channel_type, settings,
      base::BindOnce(&SimplePublishingNode::OnRequestPublish,
                     base::Unretained(this)));
}

void SimplePublishingNode::RequestUnpublish() {
  publisher_.RequestUnpublish(
      node_info_, topic_,
      base::BindOnce(&SimplePublishingNode::OnRequestUnpublish,
                     base::Unretained(this)));
}

void SimplePublishingNode::RepeatingPublish() {
  publisher_.Publish(GenerateMessage(),
                     base::BindRepeating(&SimplePublishingNode::OnPublish,
                                         base::Unretained(this)));

  if (!publisher_.IsUnregistered()) {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&SimplePublishingNode::RepeatingPublish,
                       base::Unretained(this)),
        base::TimeDelta::FromSeconds(1));
  }
}

MessageSpec SimplePublishingNode::GenerateMessage() {
  static int id = 0;
  base::TimeDelta timestamp = timestamper_.timestamp();
  MessageSpec message_spec;
  message_spec.set_id(id++);
  message_spec.set_timestamp(timestamp.InMicroseconds());
  message_spec.set_content("hello world");
  return message_spec;
}

}  // namespace felicia