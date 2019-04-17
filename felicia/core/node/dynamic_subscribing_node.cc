#include "felicia/core/node/dynamic_subscribing_node.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/master/master_proxy.h"

namespace felicia {

DynamicSubscribingNode::DynamicSubscribingNode(
    ProtobufLoader* loader, OnNewMessageCallback on_new_message_callback,
    OnSubscriptionErrorCallback on_subscription_error_callback,
    const communication::Settings& settings, const std::string& topic)
    : loader_(loader),
      topic_(topic),
      settings_(settings),
      on_new_message_callback_(on_new_message_callback),
      on_subscription_error_callback_(on_subscription_error_callback) {}

DynamicSubscribingNode::~DynamicSubscribingNode() = default;

void DynamicSubscribingNode::OnInit() {
  if (topic_.empty()) {
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.topic_info_watcher_.RegisterAllTopicCallback(
        ::base::BindRepeating(&DynamicSubscribingNode::OnFindPublisher,
                              ::base::Unretained(this)));
  }
}

void DynamicSubscribingNode::OnDidCreate(const NodeInfo& node_info) {
  node_info_ = node_info;
  if (node_info.watcher()) {
    DCHECK(topic_.empty());
    // Do nothing
  } else {
    DCHECK(!topic_.empty());
    RequestSubscribe();
  }
}

void DynamicSubscribingNode::OnError(const Status& s) {
  if (!s.ok()) {
    LOG(ERROR) << s.error_message();
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.Stop();
  }
}

void DynamicSubscribingNode::RequestSubscribe() {
  DCHECK(!node_info_.watcher());
  std::unique_ptr<DynamicSubscriber> subscriber =
      std::make_unique<DynamicSubscriber>(loader_);

  subscriber->RequestSubscribe(
      node_info_, topic_,
      ::base::BindRepeating(&DynamicSubscribingNode::OnNewMessage,
                            ::base::Unretained(this), topic_),
      ::base::BindRepeating(&DynamicSubscribingNode::OnSubscriptionError,
                            ::base::Unretained(this), topic_),
      settings_,
      ::base::BindOnce(&DynamicSubscribingNode::OnSubscriptionError,
                       ::base::Unretained(this), topic_));

  subscribers_.push_back(std::move(subscriber));
}

void DynamicSubscribingNode::OnFindPublisher(const TopicInfo& topic_info) {
  DCHECK(node_info_.watcher());
  std::unique_ptr<DynamicSubscriber> subscriber =
      std::make_unique<DynamicSubscriber>(loader_);

  subscriber->Subscribe(
      ::base::BindRepeating(&DynamicSubscribingNode::OnNewMessage,
                            ::base::Unretained(this), topic_info.topic()),
      ::base::BindRepeating(&DynamicSubscribingNode::OnSubscriptionError,
                            ::base::Unretained(this), topic_info.topic()),
      settings_);
  subscriber->OnFindPublisher(topic_info);

  subscribers_.push_back(std::move(subscriber));
}

void DynamicSubscribingNode::OnNewMessage(const std::string& topic,
                                          DynamicProtobufMessage&& message) {
  on_new_message_callback_.Run(topic, std::move(message));
}

void DynamicSubscribingNode::OnSubscriptionError(const std::string& topic,
                                                 const Status& s) {
  on_subscription_error_callback_.Run(topic, s);
}

}  // namespace felicia