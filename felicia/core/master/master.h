#ifndef FELICIA_CORE_MASTER_MASTER_H_
#define FELICIA_CORE_MASTER_MASTER_H_

#include <vector>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/errors.h"
#include "felicia/core/master/master.pb.h"
#include "felicia/core/master/master_data.pb.h"
#include "felicia/core/master/server_node.h"

namespace felicia {

class NodeNameChecker {
 public:
  explicit NodeNameChecker(const NodeInfo& node_info) : node_info_(node_info) {}

  bool operator()(const ServerNode& node);

 private:
  NodeInfo node_info_;
};

class EXPORT Master {
 public:
  void Run();
  void Stop();

  void RegisterNode(const RegisterNodeRequest* arg,
                    RegisterNodeResponse* result, StatusCallback callback);

  void UnregisterNode(const UnregisterNodeRequest* arg,
                      UnregisterNodeResponse* result, StatusCallback callback);

  void GetNodes(const GetNodesRequest* arg, GetNodesResponse* result,
                StatusCallback callback);

  void PublishTopic(const PublishTopicRequest* arg,
                    PublishTopicResponse* result, StatusCallback callback);

  void UnpublishTopic(const UnpublishTopicRequest* arg,
                      UnpublishTopicResponse* result, StatusCallback callback);

  void SubscribeTopic(const SubscribeTopicRequest* arg,
                      SubscribeTopicResponse* result, StatusCallback callback);

  void UnsubscribeTopic(const UnsubscribeTopicRequest* arg,
                        UnsubscribeTopicResponse* result,
                        StatusCallback callback);

  void Gc();

 private:
  friend class GrpcServer;

  ServerNode* FindNode(const NodeInfo& node_info);
  std::vector<ServerNode*> FindNode(const NodeFilter& node_filter);

  void AddNode(const NodeInfo& node_info);
  void RemoveNode(const NodeInfo& node_info);

  void NotifyTopicSource(std::string topic,
                         const NodeInfo& subscribing_node_info);

  Master();

  std::unique_ptr<::base::Thread> thread_;

  NodeInfo master_node_info_;

  std::vector<ServerNode> nodes_;
  std::vector<std::unique_ptr<Channel<TopicSource>>> topic_source_channels_;

  DISALLOW_COPY_AND_ASSIGN(Master);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_MASTER_H_