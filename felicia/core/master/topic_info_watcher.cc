#include "felicia/core/master/topic_info_watcher.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/core/channel/channel_factory.h"

namespace felicia {

void TopicInfoWatcher::RegisterCallback(const std::string& topic,
                                        NewTopicInfoCallback callback) {
  callback_map_[topic] = callback;
}

void TopicInfoWatcher::Start() {
  DCHECK(!channel_);
  ChannelDef channel_def;
  channel_def.set_type(ChannelDef_Type_TCP);
  channel_ = ChannelFactory::NewChannel<TopicInfo>(channel_def);

  ::base::WaitableEvent* event = new ::base::WaitableEvent;
  TCPChannel<TopicInfo>* tcp_channel = channel_->ToTCPChannel();
  tcp_channel->Listen(::base::BindOnce(&TopicInfoWatcher::OnListen,
                                       ::base::Unretained(this), event),
                      ::base::BindRepeating(&TopicInfoWatcher::OnAccept,
                                            ::base::Unretained(this)));

  event->Wait();
  delete event;
}

void TopicInfoWatcher::OnListen(::base::WaitableEvent* event,
                                const StatusOr<ChannelSource>& status_or) {
  channel_source_ = status_or.ValueOrDie();
  event->Signal();
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