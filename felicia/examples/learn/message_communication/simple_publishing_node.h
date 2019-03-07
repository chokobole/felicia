#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_SIMPLE_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_SIMPLE_PUBLISHING_NODE_H_

#include "third_party/chromium/base/time/time.h"

#include "felicia/cc/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/examples/learn/message_communication/protobuf/message_spec.pb.h"

namespace felicia {

class SimplePublishingNode : public NodeLifecycle {
 public:
  explicit SimplePublishingNode(const NodeInfo& node_info,
                                const std::string& topic,
                                const std::string& channel_type)
      : topic_(topic), publisher_(this) {
    if (channel_type.compare("TCP") == 0) {
      channel_def_.set_type(ChannelDef_Type_TCP);
    } else if (channel_type.compare("UDO") == 0) {
      channel_def_.set_type(ChannelDef_Type_UDP);
    }
  }

  void OnInit() override {
    std::cout << "SimplePublishingNode::OnInit()" << std::endl;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "SimplePublishingNode::OnDidCreate()" << std::endl;
    publisher_.set_node_info(node_info);
    ChannelDef channel_def;
    channel_def.set_type(ChannelDef_Type_TCP);
    publisher_.Publish(
        topic_,
        ::base::BindRepeating(&SimplePublishingNode::GenerateMessage,
                              ::base::Unretained(this)),
        channel_def);
  }

  MessageSpec GenerateMessage() {
    static int id = 0;
    MessageSpec message_spec;
    message_spec.set_id(id++);
    message_spec.set_timestamp(::base::Time::Now().ToInternalValue());
    message_spec.set_content("hello world");
    return message_spec;
  }

  void OnError(const Status& status) override {
    std::cout << "SimplePublishingNode::OnError()" << std::endl;
    std::cout << status.error_message() << std::endl;
  }

 private:
  std::string topic_;
  ChannelDef channel_def_;
  Publisher<MessageSpec> publisher_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_SIMPLE_PUBLISHING_NODE_H_