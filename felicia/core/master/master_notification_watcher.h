#ifndef FELICIA_CORE_MASTER_MASTER_NOTIFICATION_WATCHER_H_
#define FELICIA_CORE_MASTER_MASTER_NOTIFICATION_WATCHER_H_

#include <memory>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/macros.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {

extern Bytes kMasterNotificationBytes;

class MasterNotificationWatcher {
 public:
  using NewTopicInfoCallback = base::RepeatingCallback<void(const TopicInfo&)>;
  using NewServiceInfoCallback =
      base::RepeatingCallback<void(const ServiceInfo&)>;

  MasterNotificationWatcher();

  const ChannelSource& channel_source() const { return channel_source_; }

  void RegisterTopicInfoCallback(const std::string& topic,
                                 NewTopicInfoCallback callback);

  void UnregisterTopicInfoCallback(const std::string& topic);

  void RegisterServiceInfoCallback(const std::string& service,
                                   NewServiceInfoCallback callback);

  void UnregisterServiceInfoCallback(const std::string& service);

  void RegisterAllTopicInfoCallback(NewTopicInfoCallback callback);

  void UnregisterAllTopicInfoCallback();

  void Start();

 private:
  void DoAccept();
  void OnAccept(
      StatusOr<std::unique_ptr<TCPChannel<MasterNotification>>> status_or);

  void WatchNewMasterNotification();
  void OnNewMasterNotification(const Status& s);

  ChannelSource channel_source_;
  MasterNotification master_notificaiton_;
  std::unique_ptr<Channel<MasterNotification>> server_channel_;
  std::unique_ptr<TCPChannel<MasterNotification>> channel_;
  base::flat_map<std::string, NewTopicInfoCallback> topic_info_callback_map_;
  base::flat_map<std::string, NewServiceInfoCallback>
      service_info_callback_map_;
  NewTopicInfoCallback all_topic_info_callback_;

  DISALLOW_COPY_AND_ASSIGN(MasterNotificationWatcher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_MASTER_NOTIFICATION_WATCHER_H_