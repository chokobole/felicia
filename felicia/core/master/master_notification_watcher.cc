#include "felicia/core/master/master_notification_watcher.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/master/bytes_constants.h"

namespace felicia {

MasterNotificationWatcher::MasterNotificationWatcher() {}

void MasterNotificationWatcher::RegisterTopicInfoCallback(
    const std::string& topic, NewTopicInfoCallback callback) {
  topic_info_callback_map_[topic] = callback;
}

void MasterNotificationWatcher::UnregisterTopicInfoCallback(
    const std::string& topic) {
  topic_info_callback_map_.erase(topic_info_callback_map_.find(topic));
}

void MasterNotificationWatcher::RegisterServiceInfoCallback(
    const std::string& service, NewServiceInfoCallback callback) {
  service_info_callback_map_[service] = callback;
}

void MasterNotificationWatcher::UnregisterServiceInfoCallback(
    const std::string& service) {
  service_info_callback_map_.erase(service_info_callback_map_.find(service));
}

void MasterNotificationWatcher::RegisterAllTopicInfoCallback(
    NewTopicInfoCallback callback) {
  all_topic_info_callback_ = callback;
}

void MasterNotificationWatcher::UnregisterAllTopicInfoCallback() {
  all_topic_info_callback_.Reset();
}

void MasterNotificationWatcher::Start() {
  DCHECK(!server_channel_);
  server_channel_ = ChannelFactory::NewChannel(ChannelDef::CHANNEL_TYPE_TCP);

  TCPChannel* tcp_channel = server_channel_->ToTCPChannel();
  auto status_or = tcp_channel->Listen();
  *channel_source_.add_channel_defs() = status_or.ValueOrDie();
  DoAccept();
}

void MasterNotificationWatcher::DoAccept() {
  TCPChannel* tcp_channel = server_channel_->ToTCPChannel();
  tcp_channel->AcceptOnceIntercept(base::BindOnce(
      &MasterNotificationWatcher::OnAccept, base::Unretained(this)));
}

void MasterNotificationWatcher::OnAccept(
    StatusOr<std::unique_ptr<TCPChannel>> status_or) {
  if (status_or.ok()) {
    channel_ = std::move(status_or).ValueOrDie();
    receiver_.set_channel(channel_.get());
    WatchNewMasterNotification();
  } else {
    LOG(ERROR) << "Failed to accept: " << status_or.status();
  }
}

void MasterNotificationWatcher::WatchNewMasterNotification() {
  DCHECK(channel_);
  channel_->SetReceiveBufferSize(kMasterNotificationBytes);

  receiver_.ReceiveMessage(
      base::BindOnce(&MasterNotificationWatcher::OnNewMasterNotification,
                     base::Unretained(this)));
}

void MasterNotificationWatcher::OnNewMasterNotification(Status s) {
  if (s.ok()) {
    const MasterNotification& master_notification = receiver_.message();
    if (master_notification.has_topic_info()) {
      const TopicInfo& topic_info = master_notification.topic_info();
      auto it = topic_info_callback_map_.find(topic_info.topic());
      if (it != topic_info_callback_map_.end()) {
        it->second.Run(topic_info);
      }
      if (!all_topic_info_callback_.is_null())
        all_topic_info_callback_.Run(topic_info);
    }

    if (master_notification.has_service_info()) {
      const ServiceInfo& service_info = master_notification.service_info();
      auto it = service_info_callback_map_.find(service_info.service());
      if (it != service_info_callback_map_.end()) {
        it->second.Run(service_info);
      }
    }
  }
  DoAccept();
}

}  // namespace felicia