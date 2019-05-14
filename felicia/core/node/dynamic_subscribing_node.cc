#include "felicia/core/node/dynamic_subscribing_node.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/master/master_proxy.h"

namespace felicia {

DynamicSubscribingNode::DynamicSubscribingNode(
    ProtobufLoader* loader,
    std::unique_ptr<DynamicSubscribingNode::OneTopicDelegate> delegate)
    : loader_(loader), one_topic_delegate_(std::move(delegate)) {}

DynamicSubscribingNode::DynamicSubscribingNode(
    ProtobufLoader* loader,
    std::unique_ptr<DynamicSubscribingNode::MultiTopicDelegate> delegate)
    : loader_(loader), multi_topic_delegate_(std::move(delegate)) {}

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

  std::unique_ptr<DynamicSubscriber> subscriber =
      std::make_unique<DynamicSubscriber>(loader_);

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
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE,
        ::base::BindOnce(&DynamicSubscribingNode::Subscribe,
                         ::base::Unretained(this), topic_info, settings));
    return;
  }

  const std::string& topic = topic_info.topic();

  std::unique_ptr<DynamicSubscriber> subscriber =
      std::make_unique<DynamicSubscriber>(loader_);

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

void DynamicSubscribingNode::Unsubscribe(const TopicInfo& topic_info) {
  subscribers_.erase(topic_info.topic());
}

}  // namespace felicia