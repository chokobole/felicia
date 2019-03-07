#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_SIMPLE_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_SIMPLE_SUBSCRIBING_NODE_H_

#include "felicia/cc/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/examples/learn/message_communication/protobuf/message_spec.pb.h"

namespace felicia {

class SimpleSubscribingNode : public NodeLifecycle {
 public:
  explicit SimpleSubscribingNode(const NodeInfo& node_info,
                                 const std::string& topic)
      : topic_(topic), subscriber_(this) {}

  void OnInit() override {
    std::cout << "SimpleSubscribingNode::OnInit()" << std::endl;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    subscriber_.set_node_info(node_info);
    subscriber_.Subscribe(
        topic_, ::base::BindRepeating(&SimpleSubscribingNode::OnMessage,
                                      ::base::Unretained(this)));
    std::cout << "SimpleSubscribingNode::OnDidCreate()" << std::endl;
  }

  void OnMessage(const MessageSpec& message) {
    std::cout << "SimpleSubscribingNode::OnMessage()" << std::endl;
    std::cout << "message : " << message.DebugString() << std::endl;
  }

  void OnError(const Status& status) override {
    std::cout << "SimpleSubscribingNode::OnError()" << std::endl;
    std::cout << status.error_message() << std::endl;
  }

 private:
  std::string topic_;
  Subscriber<MessageSpec> subscriber_;
};
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_SIMPLE_SUBSCRIBING_NODE_H_