#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_PROTOBUF_CC_PROTOBUF_SUBSCRIBING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_PROTOBUF_CC_PROTOBUF_SUBSCRIBING_NODE_H_

#include "felicia/core/communication/subscriber.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/examples/learn/message_communication/common/cc/node_create_flag.h"
#include "felicia/examples/learn/message_communication/protobuf/simple_message.pb.h"

namespace felicia {

class ProtobufSubscribingNode : public NodeLifecycle {
 public:
  explicit ProtobufSubscribingNode(const NodeCreateFlag& node_create_flag);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(const NodeInfo& node_info) override;
  void OnError(const Status& s) override;

 private:
  void OnMessage(SimpleMessage&& message);
  void OnMessageError(const Status& s);
  void OnRequestSubscribe(const Status& s);
  void OnRequestUnsubscribe(const Status& s);

  void RequestSubscribe();
  void RequestUnsubscribe();

  NodeInfo node_info_;
  const NodeCreateFlag& node_create_flag_;
  const std::string topic_;
  Subscriber<SimpleMessage> subscriber_;
};
}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_PROTOBUF_CC_PROTOBUF_SUBSCRIBING_NODE_H_