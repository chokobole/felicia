#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_PROTOBUF_CC_SIMPLE_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_PROTOBUF_CC_SIMPLE_PUBLISHING_NODE_H_

#include "felicia/core/channel/socket/ssl_server_context.h"
#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/util/timestamp/timestamper.h"
#include "felicia/examples/learn/message_communication/common/cc/node_create_flag.h"
#include "felicia/examples/learn/message_communication/protobuf/message_spec.pb.h"

namespace felicia {

class SimplePublishingNode : public NodeLifecycle {
 public:
  SimplePublishingNode(const NodeCreateFlag& node_create_flag,
                       SSLServerContext* ssl_server_context);

  // NodeLifecycle methods
  void OnInit() override;
  void OnDidCreate(const NodeInfo& node_info) override;
  void OnError(const Status& s) override;

 private:
  void OnPublish(ChannelDef::Type type, const Status& s);
  void OnRequestPublish(const Status& s);
  void OnRequestUnpublish(const Status& s);

  void RequestPublish();
  void RequestUnpublish();

  void RepeatingPublish();
  MessageSpec GenerateMessage();

  NodeInfo node_info_;
  const NodeCreateFlag& node_create_flag_;
  const std::string topic_;
  Publisher<MessageSpec> publisher_;
  Timestamper timestamper_;
  SSLServerContext* ssl_server_context_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_PROTOBUF_CC_SIMPLE_PUBLISHING_NODE_H_