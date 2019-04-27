#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_PROTOBUF_CC_SIMPLE_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_PROTOBUF_CC_SIMPLE_SUBSCRIBING_NODE_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/master/master_proxy.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/examples/learn/message_communication/protobuf/message_spec.pb.h"

namespace felicia {

class SimpleSubscribingNode : public NodeLifecycle {
 public:
  SimpleSubscribingNode(const std::string& topic) : topic_(topic) {}

  void OnInit() override {
    std::cout << "SimpleSubscribingNode::OnInit()" << std::endl;
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    std::cout << "SimpleSubscribingNode::OnDidCreate()" << std::endl;
    node_info_ = node_info;
    RequestSubscribe();

    // MasterProxy& master_proxy = MasterProxy::GetInstance();
    // master_proxy.PostDelayedTask(
    //     FROM_HERE,
    //     ::base::BindOnce(&SimpleSubscribingNode::RequestUnsubscribe,
    //                      ::base::Unretained(this)),
    //     ::base::TimeDelta::FromSeconds(10));
  }

  void OnError(const Status& s) override {
    std::cout << "SimpleSubscribingNode::OnError()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestSubscribe() {
    communication::Settings settings;
    settings.buffer_size = Bytes::FromBytes(512);

    subscriber_.RequestSubscribe(
        node_info_, topic_,
        ::base::BindRepeating(&SimpleSubscribingNode::OnMessage,
                              ::base::Unretained(this)),
        ::base::BindRepeating(&SimpleSubscribingNode::OnSubscriptionError,
                              ::base::Unretained(this)),
        settings,
        ::base::BindOnce(&SimpleSubscribingNode::OnRequestSubscribe,
                         ::base::Unretained(this)));
  }

  void OnMessage(MessageSpec&& message) {
    std::cout << "SimpleSubscribingNode::OnMessage()" << std::endl;
    std::cout << "message : " << message.DebugString() << std::endl;
  }

  void OnSubscriptionError(const Status& s) {
    std::cout << "SimpleSubscribingNode::OnSubscriptionError()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void OnRequestSubscribe(const Status& s) {
    std::cout << "SimpleSubscribingNode::OnRequestSubscribe()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

  void RequestUnsubscribe() {
    subscriber_.RequestUnsubscribe(
        node_info_, topic_,
        ::base::BindOnce(&SimpleSubscribingNode::OnRequestUnsubscribe,
                         ::base::Unretained(this)));
  }

  void OnRequestUnsubscribe(const Status& s) {
    std::cout << "SimpleSubscribingNode::OnRequestUnsubscribe()" << std::endl;
    LOG_IF(ERROR, !s.ok()) << s.error_message();
  }

 private:
  NodeInfo node_info_;
  std::string topic_;
  Subscriber<MessageSpec> subscriber_;
};
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_PROTOBUF_CC_SIMPLE_SUBSCRIBING_NODE_H_