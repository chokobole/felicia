#include "felicia/core/master/master.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/lib/base/rand_util.h"
#include "felicia/core/master/heart_beat_listener.h"

namespace felicia {

namespace {

uint32_t g_client_id = 0;

}  // namespace

namespace errors {

inline ::felicia::Status ClientNotRegistered() {
  return NotFound(::base::StringPrintf("Client isn't registered yet."));
}

inline ::felicia::Status NodeNotRegistered(const NodeInfo& node_info) {
  return NotFound(::base::StringPrintf("Node '%s' isn't registered yet.",
                                       node_info.name().c_str()));
}

inline ::felicia::Status NodeAlreadyRegistered(const NodeInfo& node_info) {
  return AlreadyExists(::base::StringPrintf("Node '%s' is already registered.",
                                            node_info.name().c_str()));
}

inline ::felicia::Status TopicAlreadyPublishing(const TopicInfo& topic_info) {
  return AlreadyExists(::base::StringPrintf(
      "Topic '%s' is already being publishied.", topic_info.topic().c_str()));
}

inline ::felicia::Status FailedToPublish(const TopicInfo& topic_info) {
  return Unknown(::base::StringPrintf("Failed to publish topic '%s'.",
                                      topic_info.topic().c_str()));
}

inline ::felicia::Status TopicNotPublishingOnNode(const NodeInfo& node_info,
                                                  const std::string& topic) {
  return NotFound(::base::StringPrintf("Node '%s' isn't publishing topic '%s'.",
                                       node_info.name().c_str(),
                                       topic.c_str()));
}

inline ::felicia::Status FailedToUnpublish(const std::string& topic) {
  return Unknown(
      ::base::StringPrintf("Failed to unpublish topic '%s'.", topic.c_str()));
}

inline ::felicia::Status TopicAlreadySubscribingOnNode(
    const NodeInfo& node_info, const std::string& topic) {
  return AlreadyExists(
      ::base::StringPrintf("Node '%s' is already subscribing topic '%s'.",
                           node_info.name().c_str(), topic.c_str()));
}

inline ::felicia::Status FailedToSubscribe(const std::string& topic) {
  return Unknown(
      ::base::StringPrintf("Failed to subscribe topic '%s'.", topic.c_str()));
}

inline ::felicia::Status TopicNotSubscribingOnNode(const NodeInfo& node_info,
                                                   const std::string& topic) {
  return NotFound(
      ::base::StringPrintf("Node '%s' isn't subscribing topic '%s'.",
                           node_info.name().c_str(), topic.c_str()));
}

inline ::felicia::Status FailedToUnsubscribe(const std::string& topic) {
  return Unknown(
      ::base::StringPrintf("Failed to unsubscribe topic '%s'.", topic.c_str()));
}

}  // namespace errors

#define CHECK_CLIENT_EXISTS(node_info)                        \
  do {                                                        \
    if (!CheckIfClientExists(node_info.client_id())) {        \
      std::move(callback).Run(errors::ClientNotRegistered()); \
      return;                                                 \
    }                                                         \
  } while (false)

// General pre-check before any actions which needs |node_info|.
#define CHECK_NODE_EXISTS(node_info)                          \
  do {                                                        \
    CHECK_CLIENT_EXISTS(node_info);                           \
                                                              \
    if (!CheckIfNodeExists(node_info)) {                      \
      std::move(callback).Run(errors::ClientNotRegistered()); \
      return;                                                 \
    }                                                         \
  } while (false)

Master::Master() : thread_(std::make_unique<::base::Thread>("Main")) {}

void Master::Run() {
  thread_->StartWithOptions(
      ::base::Thread::Options{::base::MessageLoop::TYPE_IO, 0});
}

void Master::Stop() { thread_->Stop(); }

void Master::RegisterClient(const RegisterClientRequest* arg,
                            RegisterClientResponse* result,
                            StatusCallback callback) {
  g_client_id++;
  result->set_id(g_client_id);
  ClientInfo client_info = arg->client_info();
  client_info.set_id(g_client_id);
  AddClient(client_info);
  DLOG(INFO) << "[RegisterClient]: "
             << ::base::StringPrintf("client(%d)", client_info.id());
  std::move(callback).Run(Status::OK());

  thread_->task_runner()->PostTask(
      FROM_HERE, ::base::BindOnce(&Master::DoCheckHeart,
                                  ::base::Unretained(this), client_info));
}

void Master::RegisterNode(const RegisterNodeRequest* arg,
                          RegisterNodeResponse* result,
                          StatusCallback callback) {
  NodeInfo node_info = arg->node_info();
  if (node_info.name().empty()) {
    do {
      node_info.set_name(RandAlphaDigit(12));
    } while (FindNode(node_info));
  }

  if (!CheckIfClientExists(node_info.client_id())) {
    std::move(callback).Run(errors::ClientNotRegistered());
    return;
  }

  if (CheckIfNodeExists(node_info)) {
    std::move(callback).Run(errors::NodeAlreadyRegistered(node_info));
    return;
  }

  AddNode(node_info);
  *result->mutable_node_info() = node_info;
  DLOG(INFO) << "[RegisterNode]: "
             << ::base::StringPrintf("node(%s)", node_info.name().c_str());
  std::move(callback).Run(Status::OK());
}

void Master::UnregisterNode(const UnregisterNodeRequest* arg,
                            UnregisterNodeResponse* result,
                            StatusCallback callback) {
  NodeInfo node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  RemoveNode(node_info);
  DLOG(INFO) << "[UnregisterNode]: "
             << ::base::StringPrintf("node(%s)", node_info.name().c_str());
  std::move(callback).Run(Status::OK());
}

void Master::GetNodes(const GetNodesRequest* arg, GetNodesResponse* result,
                      StatusCallback callback) {
  const NodeFilter& node_filter = arg->node_filter();
  {
    ::base::AutoLock l(lock_);
    std::vector<Node*> nodes = FindNodes(node_filter);
    for (Node* node : nodes) {
      *result->add_node_infos() = node->node_info();
    }
  }
  DLOG(INFO) << "[GetNodes]";
  std::move(callback).Run(Status::OK());
}

void Master::PublishTopic(const PublishTopicRequest* arg,
                          PublishTopicResponse* result,
                          StatusCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const TopicInfo& topic_info = arg->topic_info();
  internal::Reason reason;
  {
    ::base::AutoLock l(lock_);

    NodeFilter node_filter;
    node_filter.set_publishing_topic(topic_info.topic());
    std::vector<Node*> publishing_nodes = FindNodes(node_filter);
    if (publishing_nodes.size() > 0) {
      reason = internal::Reason::TopicAlreadyPublishing;
    } else {
      Node* node = FindNode(node_info);
      if (node) {
        node->RegisterPublishingTopic(topic_info);
        reason = internal::Reason::None;
      } else {
        reason = internal::Reason::UnknownFailed;
      }
    }
  }

  if (reason == internal::Reason::None) {
    std::move(callback).Run(Status::OK());
    DLOG(INFO) << "[PublishTopic]: "
               << ::base::StringPrintf("topic(%s) from node(%s)",
                                       topic_info.topic().c_str(),
                                       node_info.name().c_str());
  } else if (reason == internal::Reason::TopicAlreadyPublishing) {
    std::move(callback).Run(errors::TopicAlreadyPublishing(topic_info));
    return;
  } else if (reason == internal::Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToPublish(topic_info));
    return;
  }

  thread_->task_runner()->PostTask(
      FROM_HERE, ::base::Bind(&Master::NotifyAllSubscribers,
                              ::base::Unretained(this), topic_info));
}

void Master::UnpublishTopic(const UnpublishTopicRequest* arg,
                            UnpublishTopicResponse* result,
                            StatusCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const std::string& topic = arg->topic();
  internal::Reason reason;
  {
    ::base::AutoLock l(lock_);
    Node* node = FindNode(node_info);
    if (node) {
      if (!node->IsPublishingTopic(topic)) {
        reason = internal::Reason::TopicNotPublishingOnNode;
      } else {
        node->UnregisterPublishingTopic(topic);
        reason = internal::Reason::None;
      }
    } else {
      reason = internal::Reason::UnknownFailed;
    }
  }

  if (reason == internal::Reason::None) {
    std::move(callback).Run(Status::OK());
    DLOG(INFO) << "[UnpublishTopic]: "
               << ::base::StringPrintf("topic(%s) from node(%s)", topic.c_str(),
                                       node_info.name().c_str());
  } else if (reason == internal::TopicNotPublishingOnNode) {
    std::move(callback).Run(errors::TopicNotPublishingOnNode(node_info, topic));
  } else if (reason == internal::Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToUnpublish(topic));
  }
}

void Master::SubscribeTopic(const SubscribeTopicRequest* arg,
                            SubscribeTopicResponse* result,
                            StatusCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const std::string& topic = arg->topic();
  internal::Reason reason;
  {
    ::base::AutoLock l(lock_);
    Node* node = FindNode(node_info);
    if (node) {
      if (node->IsSubsribingTopic(topic)) {
        reason = internal::Reason::TopicAlreadySubscribingOnNode;
      } else {
        node->RegisterSubscribingTopic(topic);
        reason = internal::Reason::None;
      }
    } else {
      reason = internal::Reason::UnknownFailed;
    }
  }

  if (reason == internal::Reason::None) {
    std::move(callback).Run(Status::OK());
    DLOG(INFO) << "[SubscribeTopic]: "
               << ::base::StringPrintf("topic(%s) from node(%s)", topic.c_str(),
                                       node_info.name().c_str());
  } else if (reason == internal::TopicNotPublishingOnNode) {
    std::move(callback).Run(
        errors::TopicAlreadySubscribingOnNode(node_info, topic));
    return;
  } else if (reason == internal::Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToSubscribe(topic));
    return;
  }

  thread_->task_runner()->PostTask(
      FROM_HERE, ::base::Bind(&Master::NotifySubscriber,
                              ::base::Unretained(this), topic, node_info));
}

void Master::UnsubscribeTopic(const UnsubscribeTopicRequest* arg,
                              UnsubscribeTopicResponse* result,
                              StatusCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const std::string& topic = arg->topic();
  internal::Reason reason;
  {
    ::base::AutoLock l(lock_);
    Node* node = FindNode(node_info);
    if (node) {
      if (node->IsSubsribingTopic(topic)) {
        node->UnregisterSubscribingTopic(topic);
        reason = internal::Reason::None;
      } else {
        reason = internal::Reason::TopicNotSubscribingOnNode;
      }
    } else {
      reason = internal::Reason::UnknownFailed;
    }
  }

  if (reason == internal::Reason::None) {
    std::move(callback).Run(Status::OK());
    DLOG(INFO) << "[UnsubscribeTopic]: "
               << ::base::StringPrintf("topic(%s) from node(%s)", topic.c_str(),
                                       node_info.name().c_str());
  } else if (reason == internal::TopicNotSubscribingOnNode) {
    std::move(callback).Run(
        errors::TopicNotSubscribingOnNode(node_info, topic));
  } else if (reason == internal::Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToUnsubscribe(topic));
  }
}

void Master::Gc() { LOG(ERROR) << "Not implemented"; }

Node* Master::FindNode(const NodeInfo& node_info) {
  auto it = client_map_.find(node_info.client_id());
  if (it == client_map_.end()) return nullptr;
  return it->second->FindNode(node_info);
}

std::vector<Node*> Master::FindNodes(const NodeFilter& node_filter) {
  std::vector<Node*> nodes;
  auto it = client_map_.begin();
  if (!node_filter.publishing_topic().empty()) {
    while (it != client_map_.end()) {
      std::vector<Node*> tmp_nodes = it->second->FindNodes(node_filter);
      nodes.insert(nodes.end(), tmp_nodes.begin(), tmp_nodes.end());
      if (nodes.size() > 0) return nodes;
      it++;
    }
  } else {
    while (it != client_map_.end()) {
      std::vector<Node*> tmp_nodes = it->second->FindNodes(node_filter);
      nodes.insert(nodes.end(), tmp_nodes.begin(), tmp_nodes.end());
      it++;
    }
  }
  return nodes;
}

void Master::AddClient(const ClientInfo& client_info) {
  DLOG(INFO) << "Master::AddClient()" << client_info.DebugString();
  ::base::AutoLock l(lock_);
  client_map_.try_emplace(client_info.id(),
                          std::make_unique<Client>(client_info));
}

void Master::RemoveClient(const ClientInfo& client_info) {
  DLOG(INFO) << "Master::RemoveClient()" << client_info.DebugString();
  ::base::AutoLock l(lock_);
  client_map_.erase(client_map_.find(client_info.id()));
}

void Master::AddNode(const NodeInfo& node_info) {
  DLOG(INFO) << "Master::AddNode()" << node_info.DebugString();
  ::base::AutoLock l(lock_);
  client_map_[node_info.client_id()]->AddNode(node_info);
}

void Master::RemoveNode(const NodeInfo& node_info) {
  DLOG(INFO) << "Master::RemoveNode()" << node_info.DebugString();
  ::base::AutoLock l(lock_);
  client_map_[node_info.client_id()]->RemoveNode(node_info);
}

bool Master::CheckIfClientExists(uint32_t id) {
  ::base::AutoLock l(lock_);
  return client_map_.find(id) != client_map_.end();
}

bool Master::CheckIfNodeExists(const NodeInfo& node_info) {
  ::base::AutoLock l(lock_);
  auto it = client_map_.find(node_info.client_id());
  if (it == client_map_.end()) return false;
  return it->second->HasNode(node_info);
}

void Master::DoNotifySubscriber(const NodeInfo& subscribing_node_info,
                                const TopicInfo& topic_info) {
  ChannelDef channel_def;
  channel_def.set_type(ChannelDef_Type_TCP);
  auto channel = ChannelFactory::NewChannel<TopicInfo>(channel_def);

  const ChannelSource& channel_source =
      client_map_[subscribing_node_info.client_id()]
          ->client_info()
          .topic_watcher_source();

  channel->Connect(channel_source,
                   ::base::BindOnce(&Master::OnConnetToTopicInfoWatcher,
                                    ::base::Unretained(this),
                                    ::base::Passed(&channel), topic_info));
}

void Master::NotifySubscriber(const std::string& topic,
                              const NodeInfo& subscribing_node_info) {
  NodeFilter node_filter;
  node_filter.set_publishing_topic(topic);
  {
    ::base::AutoLock l(lock_);
    std::vector<Node*> publishing_nodes = FindNodes(node_filter);
    if (publishing_nodes.size() > 0) {
      Node* publishing_node = publishing_nodes[0];
      DoNotifySubscriber(subscribing_node_info,
                         publishing_node->GetTopicInfo(topic));
    }
  }
}

void Master::NotifyAllSubscribers(const TopicInfo& topic_info) {
  NodeFilter node_filter;
  node_filter.set_subscribing_topic(topic_info.topic());
  {
    ::base::AutoLock l(lock_);
    std::vector<Node*> subscribing_nodes = FindNodes(node_filter);
    for (Node* subscribing_node : subscribing_nodes) {
      DoNotifySubscriber(subscribing_node->node_info(), topic_info);
    }
  }
}

void Master::OnConnetToTopicInfoWatcher(
    std::unique_ptr<Channel<TopicInfo>> channel, const TopicInfo& topic_info,
    const Status& s) {
  if (s.ok()) {
    channel->SendMessage(topic_info, ::base::BindOnce([](const Status& s) {
                           if (!s.ok()) {
                             LOG(ERROR) << "Failed to send message: "
                                        << s.error_message();
                           }
                         }));
  } else {
    LOG(ERROR) << "Failed to connect topic info channel: " << s.error_message();
  }
}

void Master::DoCheckHeart(const ClientInfo& client_info) {
  // |listner| is released inside.
  HeartBeatListener* listener = new HeartBeatListener(
      client_info,
      ::base::BindOnce(&Master::RemoveClient, ::base::Unretained(this)));
  listener->StartCheckHeart();
}

}  // namespace felicia