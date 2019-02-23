#ifndef FELICIA_CORE_MASTER_CLIENT_TOPIC_SOURCE_WATCHER_H_
#define FELICIA_CORE_MASTER_CLIENT_TOPIC_SOURCE_WATCHER_H_

#include <memory>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/location.h"
#include "third_party/chromium/base/macros.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/master/master_data.pb.h"

namespace felicia {

class EXPORT TaskRunnerInterface {
 public:
  virtual ~TaskRunnerInterface() = default;

  virtual bool PostTask(const ::base::Location& from_here,
                        ::base::OnceClosure callback) = 0;

  virtual bool PostDelayedTask(const ::base::Location& from_here,
                               ::base::OnceClosure callback,
                               ::base::TimeDelta delay) = 0;
};

class EXPORT TopicSourceWatcher {
 public:
  using OnNewTopicSourceCallback =
      ::base::RepeatingCallback<void(const TopicSource&)>;

  explicit TopicSourceWatcher(TaskRunnerInterface* task_runner_interface)
      : task_runner_interface_(task_runner_interface) {}

  void set_node_info(const NodeInfo& node_info) { node_info_ = node_info; }

  void Listen(const std::string& topic, OnNewTopicSourceCallback callback,
              const ChannelDef& channel_def = ChannelDef());

 private:
  void DoListen();
  void OnAccept(const Status& s);
  void OnListen(const StatusOr<::net::IPEndPoint>& status_or);
  void WatchNewTopicSource();
  void OnNewTopicSource(const Status& s);

  TaskRunnerInterface* task_runner_interface_;  // not owned
  NodeInfo node_info_;
  TopicSource topic_source_;
  std::unique_ptr<Channel<TopicSource>> channel_;
  ::base::flat_map<std::string, OnNewTopicSourceCallback> callback_map_;

  DISALLOW_COPY_AND_ASSIGN(TopicSourceWatcher);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_CLIENT_TOPIC_SOURCE_WATCHER_H_