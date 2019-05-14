#ifndef FELICIA_CORE_NODE_DYNAMIC_SUBSCRIBING_NODE_H_
#define FELICIA_CORE_NODE_DYNAMIC_SUBSCRIBING_NODE_H_

#include "third_party/chromium/base/containers/flat_map.h"

#include "felicia/core/communication/dynamic_subscriber.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

class DynamicSubscribingNode : public NodeLifecycle {
 public:
  class OneTopicDelegate {
   public:
    virtual ~OneTopicDelegate() = default;

    virtual void OnDidCreate(DynamicSubscribingNode* node) = 0;
    virtual void OnError(const Status& s) = 0;

    virtual void OnNewMessage(DynamicProtobufMessage&& message) = 0;
    virtual void OnSubscriptionError(const Status& s) = 0;
    virtual void OnRequestSubscribe(const Status& s) = 0;
    virtual void OnRequestUnsubscribe(const Status& s) = 0;
  };

  class MultiTopicDelegate {
   public:
    virtual ~MultiTopicDelegate() = default;

    virtual void OnDidCreate(DynamicSubscribingNode* node) = 0;
    virtual void OnError(const Status& s) = 0;

    virtual void OnNewMessage(const std::string& topic,
                              DynamicProtobufMessage&& message) = 0;
    virtual void OnSubscriptionError(const std::string& topic,
                                     const Status& s) = 0;
  };

  DynamicSubscribingNode(ProtobufLoader* loader,
                         std::unique_ptr<OneTopicDelegate> delegate);
  DynamicSubscribingNode(ProtobufLoader* loader,
                         std::unique_ptr<MultiTopicDelegate> delegate);

  ~DynamicSubscribingNode();

  void OnDidCreate(const NodeInfo& node_info) override;

  void OnError(const Status& s) override;

  void RequestSubscribe(const std::string& topic,
                        const communication::Settings& settings);

  void RequestUnubscribe(const std::string& topic);

  void Subscribe(const TopicInfo& topic_info,
                 const communication::Settings& settings);

  void Unsubscribe(const TopicInfo& topic_info);

 private:
  void OnRequestSubscribe(const std::string& topic, const Status& s);

  void OnRequestUnsubscribe(const std::string& topic, const Status& s);

  ProtobufLoader* loader_;  // not owned;
  std::unique_ptr<OneTopicDelegate> one_topic_delegate_;
  std::unique_ptr<MultiTopicDelegate> multi_topic_delegate_;
  NodeInfo node_info_;
  ::base::flat_map<std::string, std::unique_ptr<DynamicSubscriber>>
      subscribers_;

  DISALLOW_COPY_AND_ASSIGN(DynamicSubscribingNode);
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_DYNAMIC_SUBSCRIBING_NODE_H_