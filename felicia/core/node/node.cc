#include "felicia/core/node/node.h"

#include <algorithm>

#include "felicia/core/lib/strings/str_util.h"
#include "third_party/chromium/base/logging.h"

namespace felicia {

Node::Node(const NodeInfo& node_info) : node_info_(node_info) {}

void Node::RegisterPublishingTopic(const TopicInfo& topic_info) {
  topic_info_map_[topic_info.topic()] = topic_info;
}

void Node::RegisterSubscribingTopic(const std::string& topic) {
  subscribing_topics_.emplace_back(topic);
}

void Node::UnregisterPublishingTopic(const std::string& topic) {
  auto it = topic_info_map_.find(topic);
  topic_info_map_.erase(it);
}

void Node::UnregisterSubscribingTopic(const std::string& topic) {
  auto it =
      std::remove_if(subscribing_topics_.begin(), subscribing_topics_.end(),
                     strings::StringComparator{topic});
  subscribing_topics_.erase(it, subscribing_topics_.end());
}

bool Node::IsPublishingTopic(const std::string& topic) const {
  auto it = topic_info_map_.find(topic);
  return it != topic_info_map_.end();
}

bool Node::IsSubsribingTopic(const std::string& topic) const {
  auto it = std::find_if(subscribing_topics_.begin(), subscribing_topics_.end(),
                         strings::StringComparator{topic});
  return it != subscribing_topics_.end();
}

const TopicInfo& Node::GetTopicInfo(const std::string& topic) const {
  DCHECK(IsPublishingTopic(topic));
  return topic_info_map_.find(topic)->second;
}

bool NodeNameChecker::operator()(const Node& node) {
  return strings::Equals(node.name(), node_info_.name());
}

}  // namespace felicia