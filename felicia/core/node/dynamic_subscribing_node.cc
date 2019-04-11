#include "felicia/core/node/dynamic_subscribing_node.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/lib/felicia_env.h"
#include "felicia/core/master/master_proxy.h"

namespace felicia {

DynamicSubscribingNode::DynamicSubscribingNode() = default;
DynamicSubscribingNode::~DynamicSubscribingNode() = default;

void DynamicSubscribingNode::OnInit() {
  loader_ = ProtobufLoader::Load(::base::FilePath(FELICIA_ROOT));

  MasterProxy& master_proxy = MasterProxy::GetInstance();

  master_proxy.topic_info_watcher_.RegisterAllTopicCallback(
      ::base::BindRepeating(&DynamicSubscribingNode::OnFindPublisher,
                            ::base::Unretained(this)));
}

void DynamicSubscribingNode::OnDidCreate(const NodeInfo& node_info) {}

void DynamicSubscribingNode::OnError(const Status& s) {
  if (!s.ok()) {
    LOG(ERROR) << s.error_message();
    MasterProxy& master_proxy = MasterProxy::GetInstance();
    master_proxy.Stop();
  }
}

void DynamicSubscribingNode::OnFindPublisher(const TopicInfo& topic_info) {
  LOG(INFO) << topic_info.DebugString();
  std::unique_ptr<DynamicSubscriber> subscriber =
      std::make_unique<DynamicSubscriber>();

  communication::Settings settings;
  subscriber->Subscribe(
      ::base::BindRepeating(&DynamicSubscribingNode::OnNewMessage,
                            ::base::Unretained(this), topic_info.topic()),
      ::base::BindRepeating(&DynamicSubscribingNode::OnSubscriptionError,
                            ::base::Unretained(this), topic_info.topic()),
      settings, topic_info, loader_.get());
  subscriber->OnFindPublisher(topic_info);
  subscribers_.push_back(std::move(subscriber));
}

void DynamicSubscribingNode::OnNewMessage(
    const std::string& topic, const DynamicProtobufMessage& message) {
  LOG(INFO) << "topic: " << topic;
  LOG(INFO) << message.DebugString();
}

void DynamicSubscribingNode::OnSubscriptionError(const std::string& topic,
                                                 const Status& s) {
  LOG(ERROR) << "topic: " << topic;
  LOG(ERROR) << s.error_message();
}

}  // namespace felicia