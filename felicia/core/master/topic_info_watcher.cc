#include "felicia/core/master/topic_info_watcher.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/core/channel/channel_factory.h"

namespace felicia {

TopicInfoWatcher::TopicInfoWatcher() {}

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

void TopicInfoWatcher::UnregisterAllTopicCallback() {
  all_topic_callback_.Reset();
}

void TopicInfoWatcher::Start() {
  DCHECK(!server_channel_);
  server_channel_ =
      ChannelFactory::NewChannel<TopicInfo>(ChannelDef::CHANNEL_TYPE_TCP);

  TCPChannel<TopicInfo>* tcp_channel = server_channel_->ToTCPChannel();
  auto status_or = tcp_channel->Listen();
  *channel_source_.add_channel_defs() = status_or.ValueOrDie();
  DoAccept();
}

void TopicInfoWatcher::DoAccept() {
  TCPChannel<TopicInfo>* tcp_channel = server_channel_->ToTCPChannel();
  tcp_channel->AcceptOnceIntercept(
      ::base::BindOnce(&TopicInfoWatcher::OnAccept, ::base::Unretained(this)));
}

void TopicInfoWatcher::OnAccept(
    StatusOr<std::unique_ptr<TCPChannel<TopicInfo>>> status_or) {
  if (status_or.ok()) {
    channel_ = std::move(status_or.ValueOrDie());
    WatchNewTopicInfo();
  } else {
    LOG(ERROR) << "Failed to accept: " << status_or.status();
  }
}

void TopicInfoWatcher::WatchNewTopicInfo() {
  DCHECK(channel_);
  channel_->SetReceiveBufferSize(kTopicInfoBytes);

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