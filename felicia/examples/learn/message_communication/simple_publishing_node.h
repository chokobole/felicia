#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_SIMPLE_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_SIMPLE_PUBLISHING_NODE_H_

#include "third_party/chromium/base/time/time.h"

#include "felicia/cc/communication/publisher.h"
#include "felicia/cc/master_proxy.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/examples/learn/message_communication/protobuf/message_spec.pb.h"

namespace felicia {

class SimplePublishingNode : public NodeLifecycle {
 public:
  explicit SimplePublishingNode(const std::string& topic,
                                const std::string& channel_type)
      : topic_(topic) {
    if (channel_type.compare("TCP") == 0) {
      channel_def_.set_type(ChannelDef_Type_TCP);
    } else if (channel_type.compare("UDP") == 0) {
      channel_def_.set_type(ChannelDef_Type_UDP);
    }
  }

  void OnInit() override {
    std::cout << "SimplePublishingNode::OnInit()" << std::endl;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "SimplePublishingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestPublish();

    // MasterProxy& master_proxy = MasterProxy::GetInstance();
    // master_proxy.PostDelayedTask(
    //     FROM_HERE,
    //     ::base::BindOnce(&SimplePublishingNode::RequestUnpublish,
    //                      ::base::Unretained(this)),
    //     ::base::TimeDelta::FromSeconds(10));
  }

  void OnError(const Status& s) override {
    std::cout << "SimplePublishingNode::OnError()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestPublish() {
    publisher_.RequestPublish(
        node_info_, topic_, channel_def_,
        ::base::BindOnce(&SimplePublishingNode::OnRequestPublish,
                         ::base::Unretained(this)));
  }

  void OnRequestPublish(const Status& s) {
    std::cout << "SimplePublishingNode::OnRequestPublish()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
    RepeatingPublish();
  }

  void RepeatingPublish() {
    publisher_.Publish(GenerateMessage(), ::base::BindOnce(::base::BindOnce(
                                              &SimplePublishingNode::OnPublish,
                                              ::base::Unretained(this))));

    if (!publisher_.IsUnregistered()) {
      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostDelayedTask(
          FROM_HERE,
          ::base::BindOnce(&SimplePublishingNode::RepeatingPublish,
                           ::base::Unretained(this)),
          ::base::TimeDelta::FromSeconds(1));
    }
  }

  void OnPublish(const Status& s) {
    std::cout << "SimplePublishingNode::OnPublish()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  MessageSpec GenerateMessage() {
    static int id = 0;
    MessageSpec message_spec;
    message_spec.set_id(id++);
    message_spec.set_timestamp(::base::Time::Now().ToInternalValue());
    message_spec.set_content("hello world");
    return message_spec;
  }

  void RequestUnpublish() {
    std::cout << "SimplePublishingNode::RequestUnpublish()" << std::endl;
    publisher_.RequestUnpublish(
        node_info_, topic_,
        ::base::BindOnce(&SimplePublishingNode::OnRequestUnpublish,
                         ::base::Unretained(this)));
  }

  void OnRequestUnpublish(const Status& s) {
    std::cout << "SimplePublishingNode::OnRequestUnpublish()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

 private:
  NodeInfo node_info_;
  std::string topic_;
  ChannelDef channel_def_;
  Publisher<MessageSpec> publisher_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_SIMPLE_PUBLISHING_NODE_H_