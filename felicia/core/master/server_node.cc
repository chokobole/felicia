#include "felicia/core/master/server_node.h"

#include <algorithm>

#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

ServerNode::ServerNode(const NodeInfo& node_info) : node_info_(node_info) {}

::net::IPEndPoint ServerNode::net_ip_endpoint() const {
  ::net::IPAddress ip;

  ip.AssignFromIPLiteral(node_info_.ip_endpoint().ip());
  uint16_t port = node_info_.ip_endpoint().port();
  return {ip, port};
}

void ServerNode::RegisterPublishingTopic(::base::StringPiece topic,
                                         TopicSource topic_source) {
  topic_source_map_[std::string(topic)] = topic_source;
}

void ServerNode::RegisterSubscribingTopic(::base::StringPiece topic) {
  subscribing_topics_.emplace_back(topic);
}

void ServerNode::UnregisterPublishingTopic(::base::StringPiece topic) {
  auto it = topic_source_map_.find(std::string(topic));
  topic_source_map_.erase(it);
}

void ServerNode::UnregisterSubscribingTopic(::base::StringPiece topic) {
  auto it =
      std::remove_if(subscribing_topics_.begin(), subscribing_topics_.end(),
                     strings::StringComparator{topic});
  subscribing_topics_.erase(it, subscribing_topics_.end());
}

bool ServerNode::IsPublishingTopic(::base::StringPiece topic) {
  auto it = topic_source_map_.find(topic);
  return it != topic_source_map_.end();
}

bool ServerNode::IsSubsribingTopic(::base::StringPiece topic) {
  auto it = std::find_if(subscribing_topics_.begin(), subscribing_topics_.end(),
                         strings::StringComparator{topic});
  return it != subscribing_topics_.end();
}

TopicSource& ServerNode::GetTopicSource(::base::StringPiece topic) {
  DCHECK(IsPublishingTopic(topic));
  return topic_source_map_.find(std::string(topic))->second;
}

}  // namespace felicia