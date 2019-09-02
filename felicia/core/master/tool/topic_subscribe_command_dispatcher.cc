#include "felicia/core/master/tool/topic_subscribe_command_dispatcher.h"

#include "felicia/core/master/master_proxy.h"
#include "felicia/core/node/dynamic_subscribing_node.h"
#include "felicia/core/node/topic_info_watcher_node.h"

namespace felicia {

namespace {

class OneTopicSubscriberDelegate
    : public DynamicSubscribingNode::OneTopicDelegate {
 public:
  OneTopicSubscriberDelegate(const std::string& topic,
                             const communication::Settings& settings)
      : topic_(topic), settings_(settings) {}

  void OnDidCreate(DynamicSubscribingNode* node) override {
    node->RequestSubscribe(topic_, settings_);
  }

  void OnError(const Status& s) override { NOTREACHED() << s; }

  void OnMessage(DynamicProtobufMessage&& message) override {
    std::cout << message.ToString() << std::endl;
  }

  void OnMessageError(const Status& s) override {
    std::cerr << kRedError << "Message error: " << s << std::endl;
  }

  void OnRequestSubscribe(const Status& s) override { CHECK(s.ok()) << s; }

  void OnRequestUnsubscribe(const Status& s) override { CHECK(s.ok()) << s; }

 private:
  std::string topic_;
  communication::Settings settings_;
};

class MultiTopicSubscriberDelegate
    : public DynamicSubscribingNode::MultiTopicDelegate {
 public:
  MultiTopicSubscriberDelegate(const communication::Settings& settings)
      : settings_(settings) {}

  void OnDidCreate(DynamicSubscribingNode* node) override;

  void OnError(const Status& s) override { NOTREACHED() << s; }

  void OnMessage(const std::string& topic,
                 DynamicProtobufMessage&& message) override {
    std::cout << TextStyle::Green(
                     base::StringPrintf("[TOPIC] %s", topic.c_str()))
              << std::endl;
    std::cout << message.ToString() << std::endl;
  }

  void OnMessageError(const std::string& topic, const Status& s) override {
    std::cout << TextStyle::Red(base::StringPrintf("[TOPIC] %s", topic.c_str()))
              << std::endl;
    std::cerr << kRedError << "Message error: " << s << std::endl;
  }

  void HandleTopicInfo(const TopicInfo& topic_info) {
    if (topic_info.status() == TopicInfo::REGISTERED) {
      node_->Subscribe(topic_info, settings_);
    } else {
      node_->Unsubscribe(
          topic_info.topic(),
          base::BindOnce(&MultiTopicSubscriberDelegate::OnUnsubscribe,
                         base::Unretained(this)));
    }
  }

  void OnUnsubscribe(const Status& s) { LOG_IF(ERROR, !s.ok()) << s; }

 private:
  DynamicSubscribingNode* node_ = nullptr;  // not owned
  communication::Settings settings_;
};

class TopicInfoWatcherDelegate : public TopicInfoWatcherNode::Delegate {
 public:
  TopicInfoWatcherDelegate(MultiTopicSubscriberDelegate* delegate)
      : delegate_(delegate) {}

  void OnError(const Status& s) override { NOTREACHED() << s; }

  void OnNewTopicInfo(const TopicInfo& topic_info) override {
    delegate_->HandleTopicInfo(topic_info);
  }

 private:
  MultiTopicSubscriberDelegate* delegate_;  // not owned
};

void MultiTopicSubscriberDelegate::OnDidCreate(DynamicSubscribingNode* node) {
  node_ = node;
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  NodeInfo node_info;
  node_info.set_watcher(true);
  master_proxy.RequestRegisterNode<TopicInfoWatcherNode>(
      node_info, std::make_unique<TopicInfoWatcherDelegate>(this));
}

}  // namespace

TopicSubscribeCommandDispatcher::TopicSubscribeCommandDispatcher() = default;

void TopicSubscribeCommandDispatcher::Dispatch(
    const TopicSubscribeFlag& delegate) const {
  communication::Settings settings;
  if (delegate.period_flag()->is_set())
    settings.period =
        base::TimeDelta::FromMilliseconds(delegate.period_flag()->value());
  if (delegate.queue_size_flag()->is_set())
    settings.queue_size = delegate.queue_size_flag()->value();
  settings.is_dynamic_buffer = true;

  MasterProxy& master_proxy = MasterProxy::GetInstance();
  NodeInfo node_info;
  if (delegate.all_flag()->value()) {
    master_proxy.RequestRegisterNode<DynamicSubscribingNode>(
        node_info, std::make_unique<MultiTopicSubscriberDelegate>(settings));
  } else {
    master_proxy.RequestRegisterNode<DynamicSubscribingNode>(
        node_info, std::make_unique<OneTopicSubscriberDelegate>(
                       delegate.topic_flag()->value(), settings));
  }
}

}  // namespace felicia