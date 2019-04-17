#ifndef FELICIA_CORE_NODE_DYNAMIC_SUBSCRIBING_NODE_H_
#define FELICIA_CORE_NODE_DYNAMIC_SUBSCRIBING_NODE_H_

#include <memory>

#include "third_party/chromium/base/strings/string_util.h"

#include "felicia/core/communication/dynamic_subscriber.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

class DynamicSubscribingNode : public NodeLifecycle {
 public:
  using OnNewMessageCallback = ::base::RepeatingCallback<void(
      const std::string&, const DynamicProtobufMessage&)>;
  using OnSubscriptionErrorCallback =
      ::base::RepeatingCallback<void(const std::string&, const Status&)>;

  DynamicSubscribingNode(
      ProtobufLoader* loader, OnNewMessageCallback on_new_message_callback,
      OnSubscriptionErrorCallback on_subscription_error_callback,
      const communication::Settings& settings,
      const std::string& topic = ::base::EmptyString());

  ~DynamicSubscribingNode();

  void OnInit() override;

  void OnDidCreate(const NodeInfo& node_info) override;

  void OnError(const Status& s) override;

  const NodeInfo& node_info() const { return node_info_; }
  const std::string& topic() const { return topic_; }
  const std::vector<std::unique_ptr<DynamicSubscriber>>& subscribers() const {
    return subscribers_;
  }

 private:
  void RequestSubscribe();

  void OnFindPublisher(const TopicInfo& topic_info);

  void OnNewMessage(const std::string& topic,
                    const DynamicProtobufMessage& message);

  void OnSubscriptionError(const std::string& topic, const Status& s);

  ProtobufLoader* loader_;  // not owned;
  NodeInfo node_info_;
  std::string topic_;
  communication::Settings settings_;
  std::vector<std::unique_ptr<DynamicSubscriber>> subscribers_;

  OnNewMessageCallback on_new_message_callback_;
  OnSubscriptionErrorCallback on_subscription_error_callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_DYNAMIC_SUBSCRIBING_NODE_H_