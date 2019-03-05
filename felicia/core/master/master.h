#ifndef FELICIA_CORE_MASTER_MASTER_H_
#define FELICIA_CORE_MASTER_MASTER_H_

#include <memory>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/containers/flat_map.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/synchronization/lock.h"
#include "third_party/chromium/base/thread_annotations.h"
#include "third_party/chromium/base/threading/thread.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/channel/channel.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/master/client/client.h"
#include "felicia/core/master/errors.h"
#include "felicia/core/master/master.pb.h"

namespace felicia {

class EXPORT Master {
 public:
  void Run();
  void Stop();

  void RegisterClient(const RegisterClientRequest* arg,
                      RegisterClientResponse* result, StatusCallback callback);

  void RegisterNode(const RegisterNodeRequest* arg,
                    RegisterNodeResponse* result, StatusCallback callback);

  void UnregisterNode(const UnregisterNodeRequest* arg,
                      UnregisterNodeResponse* result, StatusCallback callback);

  void ListNodes(const ListNodesRequest* arg, ListNodesResponse* result,
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

  void ListTopics(const ListTopicsRequest* arg, ListTopicsResponse* result,
                  StatusCallback callback);

  void Gc();

 private:
  friend class GrpcServer;
  friend class MasterTest;
  friend std::unique_ptr<Master> NewMasterForTesting();

  enum Reason {
    None,
    TopicAlreadyPublishing,
    TopicNotPublishingOnNode,
    TopicAlreadySubscribingOnNode,
    TopicNotSubscribingOnNode,
    UnknownFailed,
  };

  // Find the node whose |node_info| is same with a given |node_info|. This is
  // thread-safe.
  ::base::WeakPtr<Node> FindNode(const NodeInfo& node_info);
  // Find the nodes which meet the given condition |node_filter|. This is
  // thread-safe.
  std::vector<::base::WeakPtr<Node>> FindNodes(const NodeFilter& node_filter);
  // Find the topics infos which meet the give condition |topic_filter|. This
  // is thread-safe.
  std::vector<TopicInfo> FindTopicInfos(const TopicFilter& topic_filter);

  // Add the Client |client|. This is thread-safe.
  void AddClient(uint32_t id, std::unique_ptr<Client> client);
  // Remove the Client whose client_info is same with |client_info|. This is
  // thread-safe.
  void RemoveClient(const ClientInfo& client_info);
  // Add the Node to the appropriate Client. This is thread-safe.
  void AddNode(std::unique_ptr<Node> node);
  // Remove the Node from the appropriate Client, which is recored at
  // |node_info|. This is thread-safe.
  void RemoveNode(const NodeInfo& node_info);
  // Returns true when client exists with a given |id|. This is thread-safe.
  bool CheckIfClientExists(uint32_t id);
  // Returns true when node whose |node_info| is same with a given |node_info|.
  // This is thread-safe.
  bool CheckIfNodeExists(const NodeInfo& node_info);

  void DoNotifySubscriber(const NodeInfo& subscribing_node_info,
                          const TopicInfo& topic_info);

  // Notify subscriber about TopicInfo which publishes |topic|.
  void NotifySubscriber(const std::string& topic,
                        const NodeInfo& subscribing_node_info);
  // Notify all the subscribers about TopicInfo |topic_info|.
  void NotifyAllSubscribers(const TopicInfo& topic_info);

  void OnConnetToTopicInfoWatcher(std::unique_ptr<Channel<TopicInfo>> channel,
                                  const TopicInfo& topic_info, const Status& s);

  // Every time a new client is registered, invoke an appropriate
  // heart beat listener for this client.
  void DoCheckHeart(const ClientInfo& client_info);

  Master();

  std::unique_ptr<::base::Thread> thread_;

  ::base::Lock lock_;
  ::base::flat_map<uint32_t, std::unique_ptr<Client>> client_map_;
  GUARDED_BY(lock_);

  DISALLOW_COPY_AND_ASSIGN(Master);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_MASTER_H_