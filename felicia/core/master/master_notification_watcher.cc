#include "felicia/core/master/master_notification_watcher.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/core/channel/channel_factory.h"

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
  server_channel_ = ChannelFactory::NewChannel<MasterNotification>(
      ChannelDef::CHANNEL_TYPE_TCP);

  TCPChannel<MasterNotification>* tcp_channel = server_channel_->ToTCPChannel();
  auto status_or = tcp_channel->Listen();
  *channel_source_.add_channel_defs() = status_or.ValueOrDie();
  DoAccept();
}

void MasterNotificationWatcher::DoAccept() {
  TCPChannel<MasterNotification>* tcp_channel = server_channel_->ToTCPChannel();
  tcp_channel->AcceptOnceIntercept(base::BindOnce(
      &MasterNotificationWatcher::OnAccept, base::Unretained(this)));
}

void MasterNotificationWatcher::OnAccept(
    StatusOr<std::unique_ptr<TCPChannel<MasterNotification>>> status_or) {
  if (status_or.ok()) {
    channel_ = std::move(status_or.ValueOrDie());
    WatchNewMasterNotification();
  } else {
    LOG(ERROR) << "Failed to accept: " << status_or.status();
  }
}

void MasterNotificationWatcher::WatchNewMasterNotification() {
  DCHECK(channel_);
  channel_->SetReceiveBufferSize(kMasterNotificationBytes);

  channel_->ReceiveMessage(
      &master_notificaiton_,
      base::BindOnce(&MasterNotificationWatcher::OnNewMasterNotification,
                     base::Unretained(this)));
}

void MasterNotificationWatcher::OnNewMasterNotification(const Status& s) {
  if (s.ok()) {
    if (master_notificaiton_.has_topic_info()) {
      const TopicInfo& topic_info = master_notificaiton_.topic_info();
      auto it = topic_info_callback_map_.find(topic_info.topic());
      if (it != topic_info_callback_map_.end()) {
        it->second.Run(topic_info);
      }
      if (!all_topic_info_callback_.is_null())
        all_topic_info_callback_.Run(topic_info);
    }

    if (master_notificaiton_.has_service_info()) {
      const ServiceInfo& service_info = master_notificaiton_.service_info();
      auto it = service_info_callback_map_.find(service_info.service());
      if (it != service_info_callback_map_.end()) {
        it->second.Run(service_info);
      }
    }
  }
  DoAccept();
}

}  // namespace felicia