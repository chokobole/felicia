#ifndef FELICIA_CORE_NODE_DYNAMIC_SUBSCRIBING_NODE_H_
#define FELICIA_CORE_NODE_DYNAMIC_SUBSCRIBING_NODE_H_

#include "third_party/chromium/base/containers/flat_map.h"

#include "felicia/core/communication/dynamic_subscriber.h"
#include "felicia/core/message/dynamic_protobuf_message.h"
#include "felicia/core/message/protobuf_loader.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

// The Difference between OneTopicDelegate and MultiTopicDelegate is
// OneTopicDelegate: it really requests to subscribe topic to the Master.
// MultiTopicDelegate: it needs a TopicInfoWatcher, which tells it if any
// updates on the topic, and it only subscribes. So Master dones't know
// if the node really subscribe any topics.
class DynamicSubscribingNode : public NodeLifecycle {
 public:
  class OneTopicDelegate {
   public:
    virtual ~OneTopicDelegate() = default;

    virtual void OnDidCreate(DynamicSubscribingNode* node) = 0;
    virtual void OnError(const Status& s) { LOG(ERROR) << s; }

    virtual void OnMessage(DynamicProtobufMessage&& message) = 0;
    virtual void OnMessageError(const Status& s) { LOG(ERROR) << s; }

    virtual void OnRequestSubscribe(const Status& s) {
      LOG_IF(ERROR, !s.ok()) << s;
    }
    virtual void OnRequestUnsubscribe(const Status& s) {
      LOG_IF(ERROR, !s.ok()) << s;
    }
  };

  class MultiTopicDelegate {
   public:
    virtual ~MultiTopicDelegate() = default;

    virtual void OnDidCreate(DynamicSubscribingNode* node) = 0;
    virtual void OnError(const Status& s) { LOG(ERROR) << s; }

    virtual void OnMessage(const std::string& topic,
                           DynamicProtobufMessage&& message) = 0;
    virtual void OnMessageError(const std::string& topic, const Status& s) {
      LOG(ERROR) << s << "from the topic " << topic;
    }
  };

  explicit DynamicSubscribingNode(std::unique_ptr<OneTopicDelegate> delegate);
  explicit DynamicSubscribingNode(std::unique_ptr<MultiTopicDelegate> delegate);

  ~DynamicSubscribingNode();

  void OnDidCreate(const NodeInfo& node_info) override;

  void OnError(const Status& s) override;

  // For OneTopicDelegate
  void RequestSubscribe(const std::string& topic,
                        const communication::Settings& settings);

  void RequestUnubscribe(const std::string& topic);

  // For MultiTopicDelegate
  void Subscribe(const TopicInfo& topic_info,
                 const communication::Settings& settings);

  void UpdateTopicInfo(const TopicInfo& topic_info);

  void Unsubscribe(const std::string& topic, StatusOnceCallback callback);

 private:
  // For OneTopicDelegate
  void OnRequestSubscribe(const std::string& topic, const Status& s);

  void OnRequestUnsubscribe(const std::string& topic, const Status& s);

  // For MultiTopicDelegate
  void OnUnsubscribe(const std::string& topic, StatusOnceCallback callback,
                     const Status& s);

  std::unique_ptr<OneTopicDelegate> one_topic_delegate_;
  std::unique_ptr<MultiTopicDelegate> multi_topic_delegate_;
  NodeInfo node_info_;
  base::flat_map<std::string, std::unique_ptr<DynamicSubscriber>> subscribers_;

  DISALLOW_COPY_AND_ASSIGN(DynamicSubscribingNode);
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_DYNAMIC_SUBSCRIBING_NODE_H_