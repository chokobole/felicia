#include "felicia/core/master/topic_info_watcher.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/core/channel/channel_factory.h"

namespace felicia {

void TopicInfoWatcher::RegisterCallback(const std::string& topic,
                                        NewTopicInfoCallback callback) {
  callback_map_[topic] = callback;
}

void TopicInfoWatcher::UnregisterCallback(const std::string& topic) {
  callback_map_.erase(callback_map_.find(topic));
}

void TopicInfoWatcher::Start() {
  DCHECK(!channel_);
  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::TCP);
  channel_ = ChannelFactory::NewChannel<TopicInfo>(channel_def);

  TCPChannel<TopicInfo>* tcp_channel = channel_->ToTCPChannel();
  auto status_or = tcp_channel->Listen();
  channel_source_ = status_or.ValueOrDie();
  tcp_channel->DoAcceptLoop(::base::BindRepeating(&TopicInfoWatcher::OnAccept,
                                                  ::base::Unretained(this)));
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
  }
}

}  // namespace felicia