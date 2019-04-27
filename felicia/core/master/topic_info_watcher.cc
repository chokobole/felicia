#include "felicia/core/master/topic_info_watcher.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/core/channel/channel_factory.h"

namespace felicia {

TopicInfoWatcher::TopicInfoWatcher(TaskRunnerInterface* task_runner_interface)
    : task_runner_interface_(task_runner_interface) {}

void TopicInfoWatcher::RegisterCallback(const std::string& topic,
                                        NewTopicInfoCallback callback) {
  callback_map_[topic] = callback;
}

void TopicInfoWatcher::UnregisterCallback(const std::string& topic) {
  callback_map_.erase(callback_map_.find(topic));
}

void TopicInfoWatcher::RegisterAllTopicCallback(NewTopicInfoCallback callback) {
  all_topic_callback_ = callback;
}

void TopicInfoWatcher::Start() {
  DCHECK(!channel_);
  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::TCP);
  channel_ = ChannelFactory::NewChannel<TopicInfo>(channel_def);

  channel_->SetReceiveBufferSize(kTopicInfoBytes);

  TCPChannel<TopicInfo>* tcp_channel = channel_->ToTCPChannel();
  auto status_or = tcp_channel->Listen();
  channel_source_ = status_or.ValueOrDie();
  DoAccept();
}

void TopicInfoWatcher::DoAccept() {
  TCPChannel<TopicInfo>* tcp_channel = channel_->ToTCPChannel();
  tcp_channel->AcceptOnce(
      ::base::BindOnce(&TopicInfoWatcher::OnAccept, ::base::Unretained(this)));
}

void TopicInfoWatcher::OnAccept(const Status& s) {
  if (s.ok()) {
    WatchNewTopicInfo();
  } else {
    LOG(ERROR) << "Failed to accept: " << s.error_message();
  }
}

void TopicInfoWatcher::WatchNewTopicInfo() {
  DCHECK(channel_);
  channel_->ReceiveMessage(&topic_info_,
                           ::base::BindOnce(&TopicInfoWatcher::OnNewTopicInfo,
                                            ::base::Unretained(this)));
}

void TopicInfoWatcher::OnNewTopicInfo(const Status& s) {
  if (s.ok()) {
    auto it = callback_map_.find(topic_info_.topic());
    if (it != callback_map_.end()) {
      it->second.Run(topic_info_);
    }
    if (!all_topic_callback_.is_null()) all_topic_callback_.Run(topic_info_);
  }
  DoAccept();
}

}  // namespace felicia