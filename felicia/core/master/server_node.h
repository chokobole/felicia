#ifndef FELICIA_CORE_MASTER_SERVER_NODE_H_
#define FELICIA_CORE_MASTER_SERVER_NODE_H_

#include <vector>

#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/net/base/ip_endpoint.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/master/master_data.pb.h"

namespace felicia {

class EXPORT ServerNode {
 public:
  explicit ServerNode(const NodeInfo& node_info);
  virtual ~ServerNode() = default;

  const NodeInfo& node_info() const { return node_info_; }
  ::base::StringPiece name() const { return node_info_.name(); }
  IPEndPoint ip_endpoint() const { return node_info_.ip_endpoint(); }
  ::net::IPEndPoint net_ip_endpoint() const;

  void RegisterPublishingTopic(TopicSource topic_source);
  void RegisterSubscribingTopic(::base::StringPiece topic);

  void UnregisterPublishingTopic(::base::StringPiece topic);
  void UnregisterSubscribingTopic(::base::StringPiece topic);

  bool IsPublishingTopic(::base::StringPiece topic);
  bool IsSubsribingTopic(::base::StringPiece topic);

  TopicSource& GetTopicSource(::base::StringPiece topic);

 protected:
  NodeInfo node_info_;
  ::base::flat_map<std::string, TopicSource> topic_source_map_;
  std::vector<std::string> subscribing_topics_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_SERVER_NODE_H_