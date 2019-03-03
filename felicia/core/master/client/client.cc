#include "felicia/core/master/client/client.h"

#include <algorithm>

namespace felicia {

void Client::AddNode(const NodeInfo& node_info) {
  DFAKE_SCOPED_LOCK(add_remove_);
  nodes_.emplace_back(node_info);
}

void Client::RemoveNode(const NodeInfo& node_info) {
  DFAKE_SCOPED_LOCK(add_remove_);
  nodes_.erase(
      std::remove_if(nodes_.begin(), nodes_.end(), NodeNameChecker{node_info}),
      nodes_.end());
}

bool Client::HasNode(const NodeInfo& node_info) const {
  DFAKE_SCOPED_LOCK(add_remove_);
  return std::find_if(nodes_.begin(), nodes_.end(),
                      NodeNameChecker{node_info}) != nodes_.end();
}

Node* Client::FindNode(const NodeInfo& node_info) {
  DFAKE_SCOPED_LOCK(add_remove_);
  auto it =
      std::find_if(nodes_.begin(), nodes_.end(), NodeNameChecker{node_info});
  if (it == nodes_.end()) {
    return nullptr;
  }

  return &(*it);
}

std::vector<Node*> Client::FindNodes(const NodeFilter& node_filter) {
  DFAKE_SCOPED_LOCK(add_remove_);
  std::vector<Node*> nodes;
  if (node_filter.all()) {
    for (auto& node : nodes_) {
      nodes.push_back(&node);
    }
  } else if (!node_filter.publishing_topic().empty()) {
    for (auto& node : nodes_) {
      if (node.IsPublishingTopic(node_filter.publishing_topic())) {
        nodes.push_back(&node);
#if !DCHECK_IS_ON()
        break;
#endif
      }
    }
    // Publishing node should be only one.
    DCHECK(nodes.size() == 0 || nodes.size() == 1);
  } else if (!node_filter.subscribing_topic().empty()) {
    for (auto& node : nodes_) {
      if (node.IsSubsribingTopic(node_filter.subscribing_topic())) {
        nodes.push_back(&node);
      }
    }
  }

  return nodes;
}

}  // namespace felicia