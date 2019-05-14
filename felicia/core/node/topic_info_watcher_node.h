#ifndef FELICIA_CORE_NODE_TOPIC_INFO_WATCHER_NODE_H_
#define FELICIA_CORE_NODE_TOPIC_INFO_WATCHER_NODE_H_

#include "felicia/core/master/topic_info_watcher.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

class TopicInfoWatcherNode : public NodeLifecycle {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;

    virtual void OnNewTopicInfo(const TopicInfo& topic_info) = 0;
    virtual void OnError(const Status& s) = 0;
  };

  TopicInfoWatcherNode(std::unique_ptr<Delegate> delegate);

  ~TopicInfoWatcherNode();

  void OnInit() override;

  void OnDidCreate(const NodeInfo& node_info) override;

  void OnError(const Status& s) override;

 private:
  std::unique_ptr<Delegate> delegate_;
  NodeInfo node_info_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_TOPIC_INFO_WATCHER_NODE_H_