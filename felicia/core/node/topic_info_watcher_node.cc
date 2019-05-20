#include "felicia/core/node/topic_info_watcher_node.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/master/master_proxy.h"

namespace felicia {

TopicInfoWatcherNode::TopicInfoWatcherNode(std::unique_ptr<Delegate> delegate)
    : delegate_(std::move(delegate)) {}

TopicInfoWatcherNode::~TopicInfoWatcherNode() = default;

void TopicInfoWatcherNode::OnInit() {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.topic_info_watcher_.RegisterAllTopicCallback(
      ::base::BindRepeating(&TopicInfoWatcherNode::Delegate::OnNewTopicInfo,
                            ::base::Unretained(delegate_.get())));
}

void TopicInfoWatcherNode::OnDidCreate(const NodeInfo& node_info) {
  DCHECK(node_info.watcher());
  node_info_ = node_info;
}

void TopicInfoWatcherNode::OnError(const Status& s) {
  LOG(ERROR) << s.error_message();
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.topic_info_watcher_.UnregisterAllTopicCallback();
  delegate_->OnError(s);
}

}  // namespace felicia