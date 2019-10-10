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
#include "felicia/core/master/client.h"
#include "felicia/core/master/errors.h"
#include "felicia/core/protobuf/master.pb.h"

namespace felicia {

extern Bytes kTopicInfoBytes;

class EXPORT Master {
 public:
  ~Master();

  void Run();
  void Stop();

#define MASTER_METHOD(method)                                       \
  void method(const method##Request* arg, method##Response* result, \
              StatusOnceCallback callback)

  MASTER_METHOD(RegisterClient);
  MASTER_METHOD(ListClients);
  MASTER_METHOD(RegisterNode);
  MASTER_METHOD(UnregisterNode);
  MASTER_METHOD(ListNodes);
  MASTER_METHOD(PublishTopic);
  MASTER_METHOD(UnpublishTopic);
  MASTER_METHOD(SubscribeTopic);
  MASTER_METHOD(UnsubscribeTopic);
  MASTER_METHOD(ListTopics);

#undef MASTER_METHOD

  void Gc();

 private:
  friend class GrpcServer;
  friend class MasterTest;
  friend class ROSMasterProxy;

  void DoRegisterClient(std::unique_ptr<Client> client,
                        StatusOnceCallback callback);
  void DoRegisterNode(std::unique_ptr<Node> node, StatusOnceCallback callback);
  void DoUnregisterNode(const NodeInfo& node_info, StatusOnceCallback callback);
  void DoListNodes(const ListNodesRequest* arg, ListNodesResponse* result,
                   StatusOnceCallback callback);
  void DoPublishTopic(const NodeInfo& node_info, const TopicInfo& topic_info,
                      StatusOnceCallback callback);
  void DoUnpublishTopic(const NodeInfo& node_info, const std::string& topic,
                        StatusOnceCallback callback);
  void DoSubscribeTopic(const NodeInfo& node_info, const std::string& topic,
                        const std::string& topic_type,
                        StatusOnceCallback callback);
  void DoUnsubscribeTopic(const NodeInfo& node_info, const std::string& topic,
                          StatusOnceCallback callback);
#if defined(HAS_ROS)
  void UnregisterROSTopics(
      const std::vector<TopicInfo>& publishing_topic_infos,
      const std::vector<std::string>& subscribing_topics) const;
#endif  // defined(HAS_ROS)

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
  base::WeakPtr<Node> FindNode(const NodeInfo& node_info);
  // Find the nodes which meet the given condition |node_filter|. This is
  // thread-safe.
  std::vector<base::WeakPtr<Node>> FindNodes(const NodeFilter& node_filter);
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
  // Returns true when a client exists with a given |id|. This is thread-safe.
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
  // Notify all the subscribers about TopicInfo in |topic_infos|.
  void NotifyAllSubscribers(const std::vector<TopicInfo>& topic_infos);
  // Notify watcher about TopicInfos which are currently being published.
  void NotifyWatcher();

  void OnConnetToTopicInfoWatcher(std::unique_ptr<Channel<TopicInfo>> channel,
                                  const TopicInfo& topic_info, const Status& s);

  void SetCheckHeartBeatForTesting(bool check_heart_beat);

  // Every time a new client is registered, invoke an appropriate
  // heart beat listener for this client.
  void DoCheckHeartBeat(const ClientInfo& client_info);

  Master();

  std::unique_ptr<base::Thread> thread_;

  base::Lock lock_;
  base::flat_map<uint32_t, std::unique_ptr<Client>> client_map_
      GUARDED_BY(lock_);

  bool check_heart_beat_ = true;

  DISALLOW_COPY_AND_ASSIGN(Master);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_MASTER_H_