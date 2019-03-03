#ifndef FELICIA_CORE_NODE_NODE_H_
#define FELICIA_CORE_NODE_NODE_H_

#include <string>
#include <vector>

#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/master/master_data.pb.h"

namespace felicia {

class EXPORT Node {
 public:
  Node() = default;

  explicit Node(const NodeInfo& node_info);
  virtual ~Node() = default;

  const std::string& name() const { return node_info_.name(); }
  const NodeInfo& node_info() const { return node_info_; }

  void RegisterPublishingTopic(const TopicInfo& topic_info);
  void RegisterSubscribingTopic(const std::string& topic);

  void UnregisterPublishingTopic(const std::string& topic);
  void UnregisterSubscribingTopic(const std::string& topic);

  bool IsPublishingTopic(const std::string& topic) const;
  bool IsSubsribingTopic(const std::string& topic) const;

  const TopicInfo& GetTopicInfo(const std::string& topic) const;

 private:
  NodeInfo node_info_;
  ::base::flat_map<std::string, TopicInfo> topic_info_map_;
  std::vector<std::string> subscribing_topics_;
};

class NodeNameChecker {
 public:
  explicit NodeNameChecker(const NodeInfo& node_info) : node_info_(node_info) {}

  bool operator()(const Node& node);

 private:
  NodeInfo node_info_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_NODE_H_