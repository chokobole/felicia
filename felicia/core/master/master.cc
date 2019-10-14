#include "felicia/core/master/master.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/master/heart_beat_listener.h"
#if defined(HAS_ROS)
#include "felicia/core/master/ros_master_proxy.h"
#endif  // defined(HAS_ROS)

namespace felicia {

Master::~Master() = default;

#define CHECK_CLIENT_EXISTS(node_info)                        \
  do {                                                        \
    if (!CheckIfClientExists(node_info.client_id())) {        \
      std::move(callback).Run(errors::ClientNotRegistered()); \
      return;                                                 \
    }                                                         \
  } while (false)

// General pre-check before any actions which needs |node_info|.
#define CHECK_NODE_EXISTS(node_info)                                 \
  do {                                                               \
    CHECK_CLIENT_EXISTS(node_info);                                  \
                                                                     \
    if (!CheckIfNodeExists(node_info)) {                             \
      std::move(callback).Run(errors::NodeNotRegistered(node_info)); \
      return;                                                        \
    }                                                                \
  } while (false)

Master::Master() : thread_(std::make_unique<base::Thread>("Master")) {}

void Master::Run() {
  thread_->StartWithOptions(
      base::Thread::Options{base::MessageLoop::TYPE_IO, 0});
}

void Master::Stop() { thread_->Stop(); }

void Master::RegisterClient(const RegisterClientRequest* arg,
                            RegisterClientResponse* result,
                            StatusOnceCallback callback) {
  const ClientInfo& client_info = arg->client_info();
  if (check_heart_beat_ &&
      !IsValidChannelSource(client_info.heart_beat_signaller_source())) {
    std::move(callback).Run(errors::ChannelSourceNotValid(
        "heart beat signaller", client_info.heart_beat_signaller_source()));
    return;
  }

  if (!IsValidChannelSource(client_info.master_notification_watcher_source())) {
    std::move(callback).Run(errors::ChannelSourceNotValid(
        "master_notification_watcher",
        client_info.master_notification_watcher_source()));
    return;
  }

  std::unique_ptr<Client> client = Client::NewClient(client_info);
  if (!client) {
    std::move(callback).Run(errors::FailedToRegisterClient());
  }

  result->set_id(client->client_info().id());
  thread_->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&Master::DoRegisterClient, base::Unretained(this),
                     std::move(client), std::move(callback)));
}

void Master::ListClients(const ListClientsRequest* arg,
                         ListClientsResponse* result,
                         StatusOnceCallback callback) {
  const ClientFilter& client_filter = arg->client_filter();
  {
    base::AutoLock l(lock_);
    if (client_filter.all()) {
      for (auto& it : client_map_) {
        *result->add_client_infos() = it.second->client_info();
      }
    } else {
      for (auto& it : client_map_) {
        if (it.second->client_info().id() == client_filter.id()) {
          *result->add_client_infos() = it.second->client_info();
          break;
        }
      }
    }
  }
  DLOG(INFO) << "[ListClients]";
  std::move(callback).Run(Status::OK());
}

void Master::RegisterNode(const RegisterNodeRequest* arg,
                          RegisterNodeResponse* result,
                          StatusOnceCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  if (!CheckIfClientExists(node_info.client_id())) {
    std::move(callback).Run(errors::ClientNotRegistered());
    return;
  }

  if (node_info.watcher()) {
    NodeFilter node_filter;
    node_filter.set_watcher(true);
    if (FindNodes(node_filter).size() != 0) {
      std::move(callback).Run(errors::WatcherNodeAlreadyRegistered());
      return;
    }
  }

  std::unique_ptr<Node> node = Node::NewNode(node_info);
  if (!node) {
    std::move(callback).Run(errors::NodeAlreadyRegistered(node_info));
    return;
  }

  *result->mutable_node_info() = node->node_info();
  thread_->task_runner()->PostTask(
      FROM_HERE, base::BindOnce(&Master::DoRegisterNode, base::Unretained(this),
                                std::move(node), std::move(callback)));
}

void Master::UnregisterNode(const UnregisterNodeRequest* arg,
                            UnregisterNodeResponse* result,
                            StatusOnceCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  thread_->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&Master::DoUnregisterNode, base::Unretained(this),
                     node_info, std::move(callback)));
}

void Master::ListNodes(const ListNodesRequest* arg, ListNodesResponse* result,
                       StatusOnceCallback callback) {
  thread_->task_runner()->PostTask(
      FROM_HERE, base::BindOnce(&Master::DoListNodes, base::Unretained(this),
                                arg, result, std::move(callback)));
}

void Master::PublishTopic(const PublishTopicRequest* arg,
                          PublishTopicResponse* result,
                          StatusOnceCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const TopicInfo& topic_info = arg->topic_info();
  if (!IsValidChannelSource(topic_info.topic_source())) {
    std::move(callback).Run(errors::ChannelSourceNotValid(
        "topic source", topic_info.topic_source()));
    return;
  }

  thread_->task_runner()->PostTask(
      FROM_HERE, base::BindOnce(&Master::DoPublishTopic, base::Unretained(this),
                                node_info, topic_info, std::move(callback)));
}

void Master::UnpublishTopic(const UnpublishTopicRequest* arg,
                            UnpublishTopicResponse* result,
                            StatusOnceCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const std::string& topic = arg->topic();
  thread_->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&Master::DoUnpublishTopic, base::Unretained(this),
                     node_info, topic, std::move(callback)));
}

void Master::SubscribeTopic(const SubscribeTopicRequest* arg,
                            SubscribeTopicResponse* result,
                            StatusOnceCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const std::string& topic = arg->topic();
  const std::string& topic_type = arg->topic_type();
  thread_->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&Master::DoSubscribeTopic, base::Unretained(this),
                     node_info, topic, topic_type, std::move(callback)));
}

void Master::UnsubscribeTopic(const UnsubscribeTopicRequest* arg,
                              UnsubscribeTopicResponse* result,
                              StatusOnceCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const std::string& topic = arg->topic();
  thread_->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&Master::DoUnsubscribeTopic, base::Unretained(this),
                     node_info, topic, std::move(callback)));
}

void Master::ListTopics(const ListTopicsRequest* arg,
                        ListTopicsResponse* result,
                        StatusOnceCallback callback) {
  const TopicFilter& topic_filter = arg->topic_filter();
  std::vector<TopicInfo> topic_infos = FindTopicInfos(topic_filter);
  for (auto& topic_info : topic_infos) {
    *result->add_topic_infos() = topic_info;
  }
  DLOG(INFO) << "[ListTopics]";
  std::move(callback).Run(Status::OK());
}

void Master::RegisterServiceClient(const RegisterServiceClientRequest* arg,
                                   RegisterServiceClientResponse* result,
                                   StatusOnceCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const std::string& service = arg->service();
  thread_->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&Master::DoRegisterServiceClient, base::Unretained(this),
                     node_info, service, std::move(callback)));
}

void Master::UnregisterServiceClient(const UnregisterServiceClientRequest* arg,
                                     UnregisterServiceClientResponse* result,
                                     StatusOnceCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const std::string& service = arg->service();
  thread_->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&Master::DoUnregisterServiceClient, base::Unretained(this),
                     node_info, service, std::move(callback)));
}

void Master::RegisterServiceServer(const RegisterServiceServerRequest* arg,
                                   RegisterServiceServerResponse* result,
                                   StatusOnceCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const ServiceInfo& service_info = arg->service_info();
  thread_->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&Master::DoRegisterServiceServer, base::Unretained(this),
                     node_info, service_info, std::move(callback)));
}

void Master::UnregisterServiceServer(const UnregisterServiceServerRequest* arg,
                                     UnregisterServiceServerResponse* result,
                                     StatusOnceCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const std::string& service = arg->service();
  thread_->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&Master::DoUnregisterServiceServer, base::Unretained(this),
                     node_info, service, std::move(callback)));
}

void Master::ListServices(const ListServicesRequest* arg,
                          ListServicesResponse* result,
                          StatusOnceCallback callback) {
  const ServiceFilter& service_filter = arg->service_filter();
  std::vector<ServiceInfo> service_infos = FindServiceInfos(service_filter);
  for (auto& service_info : service_infos) {
    *result->add_service_infos() = service_info;
  }
  DLOG(INFO) << "[ListServices]";
  std::move(callback).Run(Status::OK());
}

void Master::Gc() { LOG(ERROR) << "Not implemented"; }

void Master::DoRegisterClient(std::unique_ptr<Client> client,
                              StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  ClientInfo client_info = client->client_info();  // intend to copy
  uint32_t id = client_info.id();
  AddClient(id, std::move(client));
  DLOG(INFO) << "[RegisterClient]: " << base::StringPrintf("client(%u)", id);
  std::move(callback).Run(Status::OK());

  DoCheckHeartBeat(client_info);
}

void Master::DoRegisterNode(std::unique_ptr<Node> node,
                            StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  bool is_watcher = node->node_info().watcher();
  DLOG(INFO) << "[RegisterNode]: "
             << base::StringPrintf("node(%s)",
                                   node->node_info().name().c_str());
  AddNode(std::move(node));
  std::move(callback).Run(Status::OK());

  if (is_watcher) {
    NotifyWatcher();
  }
}

void Master::DoUnregisterNode(const NodeInfo& node_info,
                              StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  RemoveNode(node_info);
  DLOG(INFO) << "[UnregisterNode]: "
             << base::StringPrintf("node(%s)", node_info.name().c_str());
  std::move(callback).Run(Status::OK());
}

void Master::DoListNodes(const ListNodesRequest* arg, ListNodesResponse* result,
                         StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  const NodeFilter& node_filter = arg->node_filter();
  std::vector<base::WeakPtr<Node>> nodes = FindNodes(node_filter);
  if (!node_filter.name().empty()) {
    auto pub_sub_topics = result->mutable_pub_sub_topics();
    base::AutoLock l(lock_);
    if (nodes.size() > 0) {
      auto node = nodes[0];
      if (node) {
        auto publishing_topic_infos = node->AllPublishingTopicInfos();
        for (auto& publishing_topic_info : publishing_topic_infos) {
          *pub_sub_topics->add_publishing_topics() =
              publishing_topic_info.topic();
        }

        auto subscribing_topics = node->AllSubscribingTopics();
        for (auto& subscribing_topic : subscribing_topics) {
          *pub_sub_topics->add_subscribing_topics() = subscribing_topic;
        }
      }
    }
  } else {
    base::AutoLock l(lock_);
    for (auto node : nodes) {
      if (node) *result->add_node_infos() = node->node_info();
    }
  }
  DLOG(INFO) << "[ListNodes]";
  std::move(callback).Run(Status::OK());
}

void Master::DoPublishTopic(const NodeInfo& node_info,
                            const TopicInfo& topic_info,
                            StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  NodeFilter node_filter;
  node_filter.set_publishing_topic(topic_info.topic());
  std::vector<base::WeakPtr<Node>> publishing_nodes = FindNodes(node_filter);
  Reason reason;
  if (publishing_nodes.size() > 0) {
    reason = Reason::TopicAlreadyPublishing;
  } else {
    base::WeakPtr<Node> node = FindNode(node_info);
    base::AutoLock l(lock_);
    {
      if (node) {
        node->RegisterPublishingTopic(topic_info);
        reason = Reason::None;
      } else {
        reason = Reason::UnknownFailed;
      }
    }
  }

  if (reason == Reason::None) {
    DLOG(INFO) << "[PublishTopic]: "
               << base::StringPrintf("topic(%s) from node(%s)",
                                     topic_info.topic().c_str(),
                                     node_info.name().c_str());
#if defined(HAS_ROS)
    base::StringPiece t = topic_info.topic();
    if (ConsumePrefix(&t, "ros://")) {
      bool has_tcp_channel = false;
      for (const ChannelDef& channel_def :
           topic_info.topic_source().channel_defs()) {
        if (channel_def.type() == ChannelDef::CHANNEL_TYPE_TCP) {
          has_tcp_channel = true;
          break;
        }
      }
      if (!has_tcp_channel) {
        std::move(callback).Run(errors::Aborted(
            "You should set CHANNEL_TYPE_TCP on to publish ROS topic."));
        return;
      }

      ROSMasterProxy& ros_master_proxy = ROSMasterProxy::GetInstance();
      std::move(callback).Run(ros_master_proxy.RegisterPublisher(
          t.as_string(), topic_info.type_name()));
    } else {
#endif  // defined(HAS_ROS)
      std::move(callback).Run(Status::OK());
      NotifyAllSubscribers(topic_info);
#if defined(HAS_ROS)
    }
#endif  // defined(HAS_ROS)
  } else if (reason == Reason::TopicAlreadyPublishing) {
    std::move(callback).Run(errors::TopicAlreadyPublishing(topic_info));
  } else if (reason == Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToPublishTopic(topic_info));
  }
}

void Master::DoUnpublishTopic(const NodeInfo& node_info,
                              const std::string& topic,
                              StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  base::WeakPtr<Node> node = FindNode(node_info);
  TopicInfo topic_info;
  Reason reason;
  {
    base::AutoLock l(lock_);
    if (node) {
      if (!node->IsPublishingTopic(topic)) {
        reason = Reason::TopicNotPublishingOnNode;
      } else {
        topic_info = node->GetTopicInfo(topic);  // intend to copy
        node->UnregisterPublishingTopic(topic);
        reason = Reason::None;
      }
    } else {
      reason = Reason::UnknownFailed;
    }
  }

  if (reason == Reason::None) {
    DLOG(INFO) << "[UnpublishTopic]: "
               << base::StringPrintf("topic(%s) from node(%s)", topic.c_str(),
                                     node_info.name().c_str());
#if defined(HAS_ROS)
    base::StringPiece t = topic_info.topic();
    if (ConsumePrefix(&t, "ros://")) {
      ROSMasterProxy& ros_master_proxy = ROSMasterProxy::GetInstance();
      std::move(callback).Run(
          ros_master_proxy.UnregisterPublisher(t.as_string()));
    } else {
#endif  // defined(HAS_ROS)
      std::move(callback).Run(Status::OK());
      topic_info.set_status(TopicInfo::UNREGISTERED);
      NotifyAllSubscribers(topic_info);
#if defined(HAS_ROS)
    }
#endif  // defined(HAS_ROS)
  } else if (reason == Reason::TopicNotPublishingOnNode) {
    std::move(callback).Run(errors::TopicNotPublishingOnNode(node_info, topic));
  } else if (reason == Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToUnpublishTopic(topic));
  }
}

void Master::DoSubscribeTopic(const NodeInfo& node_info,
                              const std::string& topic,
                              const std::string& topic_type,
                              StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  base::WeakPtr<Node> node = FindNode(node_info);
  Reason reason;
  {
    base::AutoLock l(lock_);
    if (node) {
      if (node->IsSubsribingTopic(topic)) {
        reason = Reason::TopicAlreadySubscribingOnNode;
      } else {
        node->RegisterSubscribingTopic(topic);
        reason = Reason::None;
      }
    } else {
      reason = Reason::UnknownFailed;
    }
  }

  if (reason == Reason::None) {
    DLOG(INFO) << "[SubscribeTopic]: "
               << base::StringPrintf("topic(%s) from node(%s)", topic.c_str(),
                                     node_info.name().c_str());
#if defined(HAS_ROS)
    base::StringPiece t = topic;
    if (ConsumePrefix(&t, "ros://")) {
      ROSMasterProxy& ros_master_proxy = ROSMasterProxy::GetInstance();
      std::move(callback).Run(
          ros_master_proxy.RegisterSubscriber(t.as_string(), topic_type));
    } else {
#endif  // defined(HAS_ROS)
      std::move(callback).Run(Status::OK());
      NotifySubscriber(topic, node_info);
#if defined(HAS_ROS)
    }
#endif  // defined(HAS_ROS)
  } else if (reason == Reason::TopicAlreadySubscribingOnNode) {
    std::move(callback).Run(
        errors::TopicAlreadySubscribingOnNode(node_info, topic));
  } else if (reason == Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToSubscribeTopic(topic));
  }
}

void Master::DoUnsubscribeTopic(const NodeInfo& node_info,
                                const std::string& topic,
                                StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  base::WeakPtr<Node> node = FindNode(node_info);
  Reason reason;
  {
    base::AutoLock l(lock_);
    if (node) {
      if (node->IsSubsribingTopic(topic)) {
        node->UnregisterSubscribingTopic(topic);
        reason = Reason::None;
      } else {
        reason = Reason::TopicNotSubscribingOnNode;
      }
    } else {
      reason = Reason::UnknownFailed;
    }
  }

  if (reason == Reason::None) {
    DLOG(INFO) << "[UnsubscribeTopic]: "
               << base::StringPrintf("topic(%s) from node(%s)", topic.c_str(),
                                     node_info.name().c_str());
#if defined(HAS_ROS)
    base::StringPiece t = topic;
    if (ConsumePrefix(&t, "ros://")) {
      ROSMasterProxy& ros_master_proxy = ROSMasterProxy::GetInstance();
      std::move(callback).Run(
          ros_master_proxy.UnregisterSubscriber(t.as_string()));
    } else {
#endif  // defined(HAS_ROS)
      std::move(callback).Run(Status::OK());
#if defined(HAS_ROS)
    }
#endif  // defined(HAS_ROS)
  } else if (reason == Reason::TopicNotSubscribingOnNode) {
    std::move(callback).Run(
        errors::TopicNotSubscribingOnNode(node_info, topic));
  } else if (reason == Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToUnsubscribeTopic(topic));
  }
}

void Master::DoRegisterServiceClient(const NodeInfo& node_info,
                                     const std::string& service,
                                     StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  base::WeakPtr<Node> node = FindNode(node_info);
  Reason reason;
  {
    base::AutoLock l(lock_);
    if (node) {
      if (node->IsRequestingService(service)) {
        reason = Reason::ServiceAlreadyRequestingOnNode;
      } else {
        node->RegisterRequestingService(service);
        reason = Reason::None;
      }
    } else {
      reason = Reason::UnknownFailed;
    }
  }

  if (reason == Reason::None) {
    DLOG(INFO) << "[RegisterServiceClient]: "
               << base::StringPrintf("service(%s) from node(%s)",
                                     service.c_str(), node_info.name().c_str());
    std::move(callback).Run(Status::OK());
    NotifyServiceClient(service, node_info);
  } else if (reason == Reason::ServiceAlreadyRequestingOnNode) {
    std::move(callback).Run(
        errors::ServiceAlreadyRequestingOnNode(node_info, service));
  } else if (reason == Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToRegisterServiceClient(service));
  }
}

void Master::DoUnregisterServiceClient(const NodeInfo& node_info,
                                       const std::string& service,
                                       StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  base::WeakPtr<Node> node = FindNode(node_info);
  Reason reason;
  {
    base::AutoLock l(lock_);
    if (node) {
      if (node->IsRequestingService(service)) {
        node->UnregisterRequestingService(service);
        reason = Reason::None;
      } else {
        reason = Reason::ServiceNotRequestingOnNode;
      }
    } else {
      reason = Reason::UnknownFailed;
    }
  }

  if (reason == Reason::None) {
    DLOG(INFO) << "[UnregisterServiceClient]: "
               << base::StringPrintf("service(%s) from node(%s)",
                                     service.c_str(), node_info.name().c_str());
    std::move(callback).Run(Status::OK());
  } else if (reason == Reason::ServiceNotRequestingOnNode) {
    std::move(callback).Run(
        errors::ServiceNotRequestingOnNode(node_info, service));
  } else if (reason == Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToUnregisterServiceClient(service));
  }
}

void Master::DoRegisterServiceServer(const NodeInfo& node_info,
                                     const ServiceInfo& service_info,
                                     StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  NodeFilter node_filter;
  node_filter.set_serving_service(service_info.service());
  std::vector<base::WeakPtr<Node>> server_nodes = FindNodes(node_filter);
  Reason reason;
  if (server_nodes.size() > 0) {
    reason = Reason::ServiceAlreadyServing;
  } else {
    base::WeakPtr<Node> node = FindNode(node_info);
    base::AutoLock l(lock_);
    {
      if (node) {
        node->RegisterServingService(service_info);
        reason = Reason::None;
      } else {
        reason = Reason::UnknownFailed;
      }
    }
  }

  if (reason == Reason::None) {
    DLOG(INFO) << "[RegisterServiceServer]: "
               << base::StringPrintf("service(%s) from node(%s)",
                                     service_info.service().c_str(),
                                     node_info.name().c_str());
    std::move(callback).Run(Status::OK());
    NotifyAllServiceClients(service_info);
  } else if (reason == Reason::ServiceAlreadyServing) {
    std::move(callback).Run(
        errors::ServiceAlreadyServing(node_info, service_info));
  } else if (reason == Reason::UnknownFailed) {
    std::move(callback).Run(
        errors::FailedToRegisterServiceServer(service_info));
  }
}

void Master::DoUnregisterServiceServer(const NodeInfo& node_info,
                                       const std::string& service,
                                       StatusOnceCallback callback) {
  DCHECK(thread_->task_runner()->BelongsToCurrentThread());
  base::WeakPtr<Node> node = FindNode(node_info);
  ServiceInfo service_info;
  Reason reason;
  {
    base::AutoLock l(lock_);
    if (node) {
      if (!node->IsServingService(service)) {
        reason = Reason::ServiceNotServingOnNode;
      } else {
        service_info = node->GetServiceInfo(service);  // intend to copy
        node->UnregisterServingService(service);
        reason = Reason::None;
      }
    } else {
      reason = Reason::UnknownFailed;
    }
  }

  if (reason == Reason::None) {
    DLOG(INFO) << "[UnregisterServiceServer]: "
               << base::StringPrintf("service(%s) from node(%s)",
                                     service.c_str(), node_info.name().c_str());
    std::move(callback).Run(Status::OK());
    service_info.set_status(ServiceInfo::UNREGISTERED);
    NotifyAllServiceClients(service_info);
  } else if (reason == Reason::ServiceNotServingOnNode) {
    std::move(callback).Run(
        errors::ServiceNotServingOnNode(node_info, service));
  } else if (reason == Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToUnregisterServiceServer(service));
  }
}

#if defined(HAS_ROS)
void Master::UnregisterROSTopics(
    const std::vector<TopicInfo>& publishing_topic_infos,
    const std::vector<std::string>& subscribing_topics) const {
  ROSMasterProxy& ros_master_proxy = ROSMasterProxy::GetInstance();
  for (auto& topic_info : publishing_topic_infos) {
    base::StringPiece t = topic_info.topic();
    if (ConsumePrefix(&t, "ros://")) {
      ros_master_proxy.UnregisterPublisher(t.as_string());
    }
  }

  for (auto& topic : subscribing_topics) {
    base::StringPiece t = topic;
    if (ConsumePrefix(&t, "ros://")) {
      ros_master_proxy.UnregisterSubscriber(t.as_string());
    }
  }
}
#endif  // defined(HAS_ROS)

base::WeakPtr<Node> Master::FindNode(const NodeInfo& node_info) {
  base::AutoLock l(lock_);
  auto it = client_map_.find(node_info.client_id());
  if (it == client_map_.end()) return nullptr;
  return it->second->FindNode(node_info);
}

std::vector<base::WeakPtr<Node>> Master::FindNodes(
    const NodeFilter& node_filter) {
  base::AutoLock l(lock_);
  std::vector<base::WeakPtr<Node>> nodes;
  auto it = client_map_.begin();
  if (!node_filter.publishing_topic().empty() ||
      !node_filter.serving_service().empty()) {
    while (it != client_map_.end()) {
      std::vector<base::WeakPtr<Node>> tmp_nodes =
          it->second->FindNodes(node_filter);
      nodes.insert(nodes.end(), tmp_nodes.begin(), tmp_nodes.end());
      // Because there can be only one publishing node or server node.
      if (nodes.size() > 0) return nodes;
      it++;
    }
  } else {
    while (it != client_map_.end()) {
      std::vector<base::WeakPtr<Node>> tmp_nodes =
          it->second->FindNodes(node_filter);
      nodes.insert(nodes.end(), tmp_nodes.begin(), tmp_nodes.end());
      it++;
    }
  }
  return nodes;
}

std::vector<TopicInfo> Master::FindTopicInfos(const TopicFilter& topic_filter) {
  base::AutoLock l(lock_);
  std::vector<TopicInfo> topic_infos;
  auto it = client_map_.begin();
  if (!topic_filter.topic().empty()) {
    while (it != client_map_.end()) {
      std::vector<TopicInfo> tmp_topic_infos =
          it->second->FindTopicInfos(topic_filter);
      topic_infos.insert(topic_infos.begin(), tmp_topic_infos.begin(),
                         tmp_topic_infos.end());
      if (topic_infos.size() > 0) return topic_infos;
      it++;
    }
  } else {
    while (it != client_map_.end()) {
      std::vector<TopicInfo> tmp_topic_infos =
          it->second->FindTopicInfos(topic_filter);
      topic_infos.insert(topic_infos.begin(), tmp_topic_infos.begin(),
                         tmp_topic_infos.end());
      it++;
    }
  }
  return topic_infos;
}

std::vector<ServiceInfo> Master::FindServiceInfos(
    const ServiceFilter& service_filter) {
  base::AutoLock l(lock_);
  std::vector<ServiceInfo> service_infos;
  auto it = client_map_.begin();
  if (!service_filter.service().empty()) {
    while (it != client_map_.end()) {
      std::vector<ServiceInfo> tmp_service_infos =
          it->second->FindServiceInfos(service_filter);
      service_infos.insert(service_infos.begin(), tmp_service_infos.begin(),
                           tmp_service_infos.end());
      if (service_infos.size() > 0) return service_infos;
      it++;
    }
  } else {
    while (it != client_map_.end()) {
      std::vector<ServiceInfo> tmp_service_infos =
          it->second->FindServiceInfos(service_filter);
      service_infos.insert(service_infos.begin(), tmp_service_infos.begin(),
                           tmp_service_infos.end());
      it++;
    }
  }
  return service_infos;
}

void Master::AddClient(uint32_t id, std::unique_ptr<Client> client) {
  {
    base::AutoLock l(lock_);
    client_map_.insert_or_assign(id, std::move(client));
    DLOG(INFO) << "Master::AddClient() " << id;
  }
}

void Master::RemoveClient(const ClientInfo& client_info) {
  uint32_t id = client_info.id();

  std::vector<TopicInfo> publishing_topic_infos;
  std::vector<ServiceInfo> serving_service_infos;
#if defined(HAS_ROS)
  std::vector<std::string> subscribing_topics;
#endif
  {
    base::AutoLock l(lock_);
    auto it = client_map_.find(id);
    TopicFilter topic_filter;
    topic_filter.set_all(true);
    publishing_topic_infos = it->second->FindTopicInfos(topic_filter);
    ServiceFilter service_filter;
    service_filter.set_all(true);
    serving_service_infos = it->second->FindServiceInfos(service_filter);
#if defined(HAS_ROS)
    subscribing_topics = it->second->FindAllSubscribingTopics();
#endif
    client_map_.erase(it);
    DLOG(INFO) << "Master::RemoveClient() " << id;
  }

  for (auto& publishing_topic_info : publishing_topic_infos) {
    publishing_topic_info.set_status(TopicInfo::UNREGISTERED);
  }

  for (auto& serving_service_info : serving_service_infos) {
    serving_service_info.set_status(ServiceInfo::UNREGISTERED);
  }

  NotifyAllSubscribers(publishing_topic_infos);
  NotifyAllServiceClients(serving_service_infos);

#if defined(HAS_ROS)
  UnregisterROSTopics(publishing_topic_infos, subscribing_topics);
#endif  // defined(HAS_ROS)
}

void Master::AddNode(std::unique_ptr<Node> node) {
  uint32_t id = node->node_info().client_id();
  {
    base::AutoLock l(lock_);
    auto it = client_map_.find(id);
    if (it != client_map_.end()) {
      DLOG(INFO) << "Master::AddNode() " << node->node_info().name();
      it->second->AddNode(std::move(node));
    }
  }
}

void Master::RemoveNode(const NodeInfo& node_info) {
  uint32_t id = node_info.client_id();
  std::vector<TopicInfo> publishing_topic_infos;
  std::vector<ServiceInfo> serving_service_infos;
#if defined(HAS_ROS)
  std::vector<std::string> subscribing_topics;
#endif  // defined(HAS_ROS)
  {
    base::AutoLock l(lock_);
    auto it = client_map_.find(id);
    if (it != client_map_.end()) {
      base::WeakPtr<Node> node = it->second->FindNode(node_info);
      if (node) {
        publishing_topic_infos = node->AllPublishingTopicInfos();
        serving_service_infos = node->AllServingServiceInfos();
#if defined(HAS_ROS)
        subscribing_topics = node->AllSubscribingTopics();
#endif  // defined(HAS_ROS)
      }
      it->second->RemoveNode(node_info);
      DLOG(INFO) << "Master::RemoveNode() " << node_info.name();
    }
  }

  for (auto& publishing_topic_info : publishing_topic_infos) {
    publishing_topic_info.set_status(TopicInfo::UNREGISTERED);
  }

  for (auto& serving_service_info : serving_service_infos) {
    serving_service_info.set_status(ServiceInfo::UNREGISTERED);
  }

  NotifyAllSubscribers(publishing_topic_infos);
  NotifyAllServiceClients(serving_service_infos);

#if defined(HAS_ROS)
  UnregisterROSTopics(publishing_topic_infos, subscribing_topics);
#endif  // defined(HAS_ROS)
}

bool Master::CheckIfClientExists(uint32_t id) {
  base::AutoLock l(lock_);
  return client_map_.find(id) != client_map_.end();
}

bool Master::CheckIfNodeExists(const NodeInfo& node_info) {
  base::AutoLock l(lock_);
  auto it = client_map_.find(node_info.client_id());
  if (it == client_map_.end()) return false;
  return it->second->HasNode(node_info);
}

void Master::DoNotifyClient(const NodeInfo& node_info,
                            const MasterNotification& master_notification) {
  // TODO(chokobole): Try not make one channel for each |master_notification|.
  auto channel = ChannelFactory::NewChannel<MasterNotification>(
      ChannelDef::CHANNEL_TYPE_TCP);

  channel->SetSendBufferSize(kMasterNotificationBytes);

  ChannelSource channel_source;
  {
    base::AutoLock l(lock_);
    auto it = client_map_.find(node_info.client_id());
    if (it == client_map_.end()) return;
    channel_source =
        it->second->client_info().master_notification_watcher_source();
  }

  DCHECK_EQ(channel_source.channel_defs_size(), 1);
  DCHECK_EQ(channel_source.channel_defs(0).type(),
            ChannelDef::CHANNEL_TYPE_TCP);

  channel->Connect(channel_source.channel_defs(0),
                   base::BindOnce(&Master::OnConnetToMasterNotificationWatcher,
                                  base::Unretained(this),
                                  base::Passed(&channel), master_notification));
}

void Master::NotifySubscriber(const std::string& topic,
                              const NodeInfo& subscribing_node_info) {
  if (!thread_->task_runner()->BelongsToCurrentThread()) {
    thread_->task_runner()->PostTask(
        FROM_HERE, base::Bind(&Master::NotifySubscriber, base::Unretained(this),
                              topic, subscribing_node_info));
    return;
  }
  NodeFilter node_filter;
  node_filter.set_publishing_topic(topic);
  std::vector<base::WeakPtr<Node>> publishing_nodes = FindNodes(node_filter);
  if (publishing_nodes.size() > 0) {
    base::WeakPtr<Node> publishing_node = publishing_nodes[0];
    if (publishing_node) {
      MasterNotification master_notification;
      *master_notification.mutable_topic_info() =
          publishing_node->GetTopicInfo(topic);
      DoNotifyClient(subscribing_node_info, master_notification);
    }
  }
}

void Master::NotifyAllSubscribers(const TopicInfo& topic_info) {
  if (!thread_->task_runner()->BelongsToCurrentThread()) {
    thread_->task_runner()->PostTask(
        FROM_HERE, base::Bind((void (Master::*)(const TopicInfo&)) &
                                  Master::NotifyAllSubscribers,
                              base::Unretained(this), topic_info));
    return;
  }
  NodeFilter node_filter;
  node_filter.set_subscribing_topic(topic_info.topic());
  std::vector<base::WeakPtr<Node>> subscribing_nodes = FindNodes(node_filter);
  node_filter.Clear();
  node_filter.set_watcher(true);
  std::vector<base::WeakPtr<Node>> watcher_nodes = FindNodes(node_filter);
  subscribing_nodes.insert(subscribing_nodes.end(), watcher_nodes.begin(),
                           watcher_nodes.end());

  MasterNotification master_notification;
  *master_notification.mutable_topic_info() = topic_info;
  for (auto& subscribing_node : subscribing_nodes) {
    if (subscribing_node)
      DoNotifyClient(subscribing_node->node_info(), master_notification);
  }
}

void Master::NotifyAllSubscribers(const std::vector<TopicInfo>& topic_infos) {
  for (auto& topic_info : topic_infos) {
    NotifyAllSubscribers(topic_info);
  }
}

void Master::NotifyServiceClient(const std::string& service,
                                 const NodeInfo& client_node_info) {
  if (!thread_->task_runner()->BelongsToCurrentThread()) {
    thread_->task_runner()->PostTask(
        FROM_HERE,
        base::Bind(&Master::NotifyServiceClient, base::Unretained(this),
                   service, client_node_info));
    return;
  }
  NodeFilter node_filter;
  node_filter.set_serving_service(service);
  std::vector<base::WeakPtr<Node>> server_nodes = FindNodes(node_filter);
  if (server_nodes.size() > 0) {
    base::WeakPtr<Node> server_node = server_nodes[0];
    if (server_node) {
      MasterNotification master_notification;
      *master_notification.mutable_service_info() =
          server_node->GetServiceInfo(service);
      DoNotifyClient(client_node_info, master_notification);
    }
  }
}

void Master::NotifyAllServiceClients(const ServiceInfo& service_info) {
  if (!thread_->task_runner()->BelongsToCurrentThread()) {
    thread_->task_runner()->PostTask(
        FROM_HERE, base::Bind((void (Master::*)(const ServiceInfo&)) &
                                  Master::NotifyAllServiceClients,
                              base::Unretained(this), service_info));
    return;
  }
  NodeFilter node_filter;
  node_filter.set_requesting_service(service_info.service());
  std::vector<base::WeakPtr<Node>> client_nodes = FindNodes(node_filter);
  node_filter.Clear();

  MasterNotification master_notification;
  *master_notification.mutable_service_info() = service_info;
  for (auto& client_node : client_nodes) {
    if (client_node)
      DoNotifyClient(client_node->node_info(), master_notification);
  }
}

void Master::NotifyAllServiceClients(
    const std::vector<ServiceInfo>& service_infos) {
  for (auto& service_info : service_infos) {
    NotifyAllServiceClients(service_info);
  }
}

void Master::NotifyWatcher() {
  NodeFilter node_filter;
  node_filter.set_watcher(true);
  std::vector<base::WeakPtr<Node>> watcher_nodes = FindNodes(node_filter);
  TopicFilter topic_filter;
  topic_filter.set_all(true);
  std::vector<TopicInfo> topic_infos = FindTopicInfos(topic_filter);
  if (watcher_nodes.size() > 0) {
    base::WeakPtr<Node> watcher_node = watcher_nodes[0];
    if (watcher_node) {
      for (TopicInfo& topic_info : topic_infos) {
        MasterNotification master_notification;
        *master_notification.mutable_topic_info() = topic_info;
        DoNotifyClient(watcher_node->node_info(), master_notification);
      }
    }
  }
}

void Master::OnConnetToMasterNotificationWatcher(
    std::unique_ptr<Channel<MasterNotification>> channel,
    const MasterNotification& master_notification, const Status& s) {
  if (s.ok()) {
    channel->SendMessage(
        master_notification,
        base::BindRepeating([](ChannelDef::Type type, const Status& s) {
          LOG_IF(ERROR, !s.ok()) << "Failed to send message: " << s;
        }));
  } else {
    LOG(ERROR) << "Failed to connect master notification channel: " << s;
  }
}

void Master::SetCheckHeartBeatForTesting(bool check_heart_beat) {
  check_heart_beat_ = check_heart_beat;
}

void Master::DoCheckHeartBeat(const ClientInfo& client_info) {
  if (!check_heart_beat_) return;
  // |listner| is released inside.
  HeartBeatListener* listener = new HeartBeatListener(
      client_info,
      base::BindOnce(&Master::RemoveClient, base::Unretained(this)));
  listener->StartCheckHeartBeat();
}

#undef CHECK_CLIENT_EXISTS
#undef CHECK_NODE_EXISTS

}  // namespace felicia