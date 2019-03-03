#ifndef FELICIA_CORE_MASTER_TOPIC_INFO_WATCHER_H_
#define FELICIA_CORE_MASTER_TOPIC_INFO_WATCHER_H_

#include <memory>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/synchronization/waitable_event.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/master/master_data.pb.h"
#include "felicia/core/master/task_runner_interface.h"

namespace felicia {

class TopicInfoWatcher {
 public:
  using NewTopicInfoCallback =
      ::base::RepeatingCallback<void(const TopicInfo&)>;

  explicit TopicInfoWatcher(TaskRunnerInterface* task_runner_interface)
      : task_runner_interface_(task_runner_interface) {}

  const ChannelSource& channel_source() const { return channel_source_; }

  void RegisterCallback(const std::string& topic,
                        NewTopicInfoCallback callback);

  void Start();

 private:
  void OnListen(::base::WaitableEvent* event,
                const StatusOr<ChannelSource>& status_or);

  void OnAccept(const Status& s);

  void WatchNewTopicInfo();
  void OnNewTopicInfo(const Status& s);

  TaskRunnerInterface* task_runner_interface_;  // not owned
  ChannelSource channel_source_;
  TopicInfo topic_info_;
  std::unique_ptr<Channel<TopicInfo>> channel_;
  ::base::flat_map<std::string, NewTopicInfoCallback> callback_map_;

  DISALLOW_COPY_AND_ASSIGN(TopicInfoWatcher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TOPIC_INFO_WATCHER_H_