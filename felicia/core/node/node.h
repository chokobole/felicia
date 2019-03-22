#ifndef FELICIA_CORE_NODE_NODE_H_
#define FELICIA_CORE_NODE_NODE_H_

#include <memory>
#include <string>
#include <vector>

#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/memory/weak_ptr.h"

#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {

class Node : public ::base::SupportsWeakPtr<Node> {
 public:
  // Return a new node unless a |node_info| contains name and there is
  // already registered with a given name. If so, return nullptr.
  static std::unique_ptr<Node> NewNode(const NodeInfo& node_info);

  ~Node();

  const std::string& name() const { return node_info_.name(); }
  const NodeInfo& node_info() const { return node_info_; }

  void RegisterPublishingTopic(const TopicInfo& topic_info);
  void RegisterSubscribingTopic(const std::string& topic);

  void UnregisterPublishingTopic(const std::string& topic);
  void UnregisterSubscribingTopic(const std::string& topic);

  bool IsPublishingTopic(const std::string& topic) const;
  bool IsSubsribingTopic(const std::string& topic) const;

  const TopicInfo& GetTopicInfo(const std::string& topic) const;
  std::vector<TopicInfo> AllPublishingTopicInfos() const;
  std::vector<std::string> AllSubscribingTopics() const;

 private:
  explicit Node(const NodeInfo& node_info);

  NodeInfo node_info_;
  ::base::flat_map<std::string, TopicInfo> topic_info_map_;
  std::vector<std::string> subscribing_topics_;

  DISALLOW_COPY_AND_ASSIGN(Node);
};

class NodeNameChecker {
 public:
  explicit NodeNameChecker(const NodeInfo& node_info) : node_info_(node_info) {}

  bool operator()(const std::unique_ptr<Node>& node);

 private:
  NodeInfo node_info_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_NODE_H_