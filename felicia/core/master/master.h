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

extern Bytes kMasterNotificationBytes;

class EXPORT Master {
 public:
  ~Master();

  void Run();
  void Stop();

#define MASTER_METHOD(Method, method, cancelable)                   \
  void Method(const Method##Request* arg, Method##Response* result, \
              StatusOnceCallback callback);
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD

  void Gc();

 private:
  friend class MasterServer;
  friend class MasterTest;
  friend class RosMasterProxy;

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
  void DoRegisterServiceClient(const NodeInfo& node_info,
                               const std::string& service,
                               StatusOnceCallback callback);
  void DoUnregisterServiceClient(const NodeInfo& node_info,
                                 const std::string& service,
                                 StatusOnceCallback callback);
  void DoRegisterServiceServer(const NodeInfo& node_info,
                               const ServiceInfo& service_info,
                               StatusOnceCallback callback);
  void DoUnregisterServiceServer(const NodeInfo& node_info,
                                 const std::string& service,
                                 StatusOnceCallback callback);
#if defined(HAS_ROS)
  void UnregisterRosTopicsAndServices(
      const std::vector<TopicInfo>& publishing_topic_infos,
      const std::vector<std::string>& subscribing_topics,
      const std::vector<ServiceInfo>& serving_service_infos) const;
#endif  // defined(HAS_ROS)

  enum Reason {
    None,
    TopicAlreadyPublishingOnNode,
    TopicNotPublishingOnNode,
    TopicAlreadySubscribingOnNode,
    TopicNotSubscribingOnNode,
    ServiceAlreadyServingOnNode,
    ServiceNotServingOnNode,
    ServiceAlreadyRequestingOnNode,
    ServiceNotRequestingOnNode,
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
  // Find the service infos which meet the give condition |service_filter|. This
  // is thread-safe.
  std::vector<ServiceInfo> FindServiceInfos(
      const ServiceFilter& service_filter);

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

  void DoNotifyClient(const NodeInfo& node_info,
                      const MasterNotification& master_notification);

  // Notify subscriber about TopicInfo which publishes |topic|.
  void NotifySubscriber(const std::string& topic,
                        const NodeInfo& subscribing_node_info);
  // Notify all the subscribers about TopicInfo |topic_info|.
  void NotifyAllSubscribers(const TopicInfo& topic_info);
  // Notify all the subscribers about TopicInfo in |topic_infos|.
  void NotifyAllSubscribers(const std::vector<TopicInfo>& topic_infos);
  // Notify ServiceClient about ServiceInfo which serves |service|.
  void NotifyServiceClient(const std::string& service,
                           const NodeInfo& client_node_info);
  // Notify all the ServiceClients about ServiceInfo |service_info|.
  void NotifyAllServiceClients(const ServiceInfo& service_info);
  // Notify all the ServiceClients about ServiceInfo in |service_infos|.
  void NotifyAllServiceClients(const std::vector<ServiceInfo>& service_infos);
  // Notify watcher about TopicInfos which are currently being published.
  void NotifyWatcher();

  void OnConnetToMasterNotificationWatcher(
      std::unique_ptr<Channel<MasterNotification>> channel,
      const MasterNotification& master_notification, const Status& s);

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