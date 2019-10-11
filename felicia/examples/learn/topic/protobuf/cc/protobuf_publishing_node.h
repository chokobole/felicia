#ifndef FELICIA_EXAMPLES_LEARN_TOPIC_PROTOBUF_CC_PROTOBUF_PUBLISHING_NODE_H_
#define FELICIA_EXAMPLES_LEARN_TOPIC_PROTOBUF_CC_PROTOBUF_PUBLISHING_NODE_H_

#include "felicia/core/channel/socket/ssl_server_context.h"
#include "felicia/core/communication/publisher.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/util/timestamp/timestamper.h"
#include "felicia/examples/learn/topic/common/cc/topic_create_flag.h"
#include "felicia/examples/learn/topic/protobuf/simple_message.pb.h"

namespace felicia {

class ProtobufPublishingNode : public NodeLifecycle {
 public:
  ProtobufPublishingNode(const TopicCreateFlag& topic_create_flag,
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
  SimpleMessage GenerateMessage();

  NodeInfo node_info_;
  const TopicCreateFlag& topic_create_flag_;
  const std::string topic_;
  Publisher<SimpleMessage> publisher_;
  Timestamper timestamper_;
  SSLServerContext* ssl_server_context_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_TOPIC_PROTOBUF_CC_PROTOBUF_PUBLISHING_NODE_H_