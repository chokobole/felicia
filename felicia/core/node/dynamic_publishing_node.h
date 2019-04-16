#ifndef FELICIA_CORE_NODE_DYNAMIC_PUBLISHING_NODE_H_
#define FELICIA_CORE_NODE_DYNAMIC_PUBLISHING_NODE_H_

#include <memory>

#include "felicia/core/communication/dynamic_publisher.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

class DynamicPublishingNode : public NodeLifecycle {
 public:
  using PublisherCallback = ::base::OnceCallback<void(DynamicPublisher*)>;

  DynamicPublishingNode(ProtobufLoader* loader, const std::string& topic,
                        const std::string& message_type,
                        const ChannelDef& channel_def,
                        PublisherCallback publisher_callback);

  ~DynamicPublishingNode();

  void OnInit() override;

  void OnDidCreate(const NodeInfo& node_info) override;

  void OnError(const Status& s) override;

  const std::string& topic() const { return topic_; }
  const std::string& message_type() const { return message_type_; }
  const ChannelDef& channel_def() const { return channel_def_; }

  void PublishMessageFromJSON(const std::string& json_message,
                              StatusOnceCallback callback);

 private:
  void RequestPublish();

  void OnRequestPublish(const Status& s);

  ProtobufLoader* loader_;  // not owned;
  NodeInfo node_info_;
  std::string topic_;
  std::string message_type_;
  ChannelDef channel_def_;
  std::unique_ptr<DynamicPublisher> publisher_;

  PublisherCallback publisher_callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_DYNAMIC_PUBLISHING_NODE_H_