#include "felicia/core/node/dynamic_subscribing_node.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/master/master_proxy.h"

namespace felicia {

DynamicSubscribingNode::DynamicSubscribingNode(
    std::unique_ptr<DynamicSubscribingNode::OneTopicDelegate> delegate)
    : one_topic_delegate_(std::move(delegate)) {}

DynamicSubscribingNode::DynamicSubscribingNode(
    std::unique_ptr<DynamicSubscribingNode::MultiTopicDelegate> delegate)
    : multi_topic_delegate_(std::move(delegate)) {}

DynamicSubscribingNode::~DynamicSubscribingNode() = default;

void DynamicSubscribingNode::OnDidCreate(const NodeInfo& node_info) {
  node_info_ = node_info;
  if (one_topic_delegate_) {
    one_topic_delegate_->OnDidCreate(this);
  } else {
    multi_topic_delegate_->OnDidCreate(this);
  }
}

void DynamicSubscribingNode::OnError(const Status& s) {
  if (one_topic_delegate_) {
    one_topic_delegate_->OnError(s);
  } else {
    multi_topic_delegate_->OnError(s);
  }
}

void DynamicSubscribingNode::RequestSubscribe(
    const std::string& topic, const communication::Settings& settings) {
  if (subscribers_.find(topic) != subscribers_.end()) {
    one_topic_delegate_->OnRequestSubscribe(errors::AlreadyExists(
        ::base::StrCat({"Already registered with topic", topic})));
    return;
  }

  auto subscriber = std::make_unique<DynamicSubscriber>();

  subscriber->RequestSubscribe(
      node_info_, topic,
      ::base::BindRepeating(
          &DynamicSubscribingNode::OneTopicDelegate::OnNewMessage,
          ::base::Unretained(one_topic_delegate_.get())),
      ::base::BindRepeating(
          &DynamicSubscribingNode::OneTopicDelegate::OnSubscriptionError,
          ::base::Unretained(one_topic_delegate_.get())),
      settings,
      ::base::BindOnce(&DynamicSubscribingNode::OnRequestSubscribe,
                       ::base::Unretained(this), topic));

  subscribers_[topic] = std::move(subscriber);
}

void DynamicSubscribingNode::OnRequestSubscribe(const std::string& topic,
                                                const Status& s) {
  if (!s.ok()) {
    subscribers_.erase(topic);
  }

  one_topic_delegate_->OnRequestSubscribe(s);
}

void DynamicSubscribingNode::RequestUnubscribe(const std::string& topic) {
  auto it = subscribers_.find(topic);
  if (it == subscribers_.end()) return;

  (*it).second->RequestUnsubscribe(
      node_info_, topic,
      ::base::BindOnce(&DynamicSubscribingNode::OnRequestUnsubscribe,
                       ::base::Unretained(this), topic));
}

void DynamicSubscribingNode::OnRequestUnsubscribe(const std::string& topic,
                                                  const Status& s) {
  if (s.ok()) {
    subscribers_.erase(topic);
  }

  one_topic_delegate_->OnRequestUnsubscribe(s);
}

void DynamicSubscribingNode::Subscribe(
    const TopicInfo& topic_info, const communication::Settings& settings) {
  const std::string& topic = topic_info.topic();
  auto it = subscribers_.find(topic);
  if (it != subscribers_.end()) {
    if (it->second->IsStopping()) {
      MasterProxy& master_proxy = MasterProxy::GetInstance();
      master_proxy.PostTask(
          FROM_HERE,
          ::base::BindOnce(&DynamicSubscribingNode::Subscribe,
                           ::base::Unretained(this), topic_info, settings));
      return;
    }
  }

  auto subscriber = std::make_unique<DynamicSubscriber>();

  subscriber->Subscribe(
      ::base::BindRepeating(
          &DynamicSubscribingNode::MultiTopicDelegate::OnNewMessage,
          ::base::Unretained(multi_topic_delegate_.get()), topic),
      ::base::BindRepeating(
          &DynamicSubscribingNode::MultiTopicDelegate::OnSubscriptionError,
          ::base::Unretained(multi_topic_delegate_.get()), topic),
      settings);
  subscriber->OnFindPublisher(topic_info);

  subscribers_[topic] = std::move(subscriber);
}

void DynamicSubscribingNode::UpdateTopicInfo(const TopicInfo& topic_info) {
  auto it = subscribers_.find(topic_info.topic());
  if (it == subscribers_.end()) return;

  const TopicInfo& cur_topic_info = it->second->topic_info();
  if (cur_topic_info.type_name() == topic_info.type_name()) {
    it->second->OnFindPublisher(topic_info);
  }
}

void DynamicSubscribingNode::Unsubscribe(const std::string& topic,
                                         StatusOnceCallback callback) {
  auto it = subscribers_.find(topic);
  if (it == subscribers_.end()) return;

  it->second->UnSubscribe(
      topic,
      ::base::BindOnce(&DynamicSubscribingNode::OnUnsubscribe,
                       ::base::Unretained(this), topic, std::move(callback)));
}

void DynamicSubscribingNode::OnUnsubscribe(const std::string& topic,
                                           StatusOnceCallback callback,
                                           const Status& s) {
  auto it = subscribers_.find(topic);
  subscribers_.erase(it);

  std::move(callback).Run(s);
}

}  // namespace felicia