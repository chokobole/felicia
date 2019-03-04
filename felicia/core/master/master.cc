#include "felicia/core/master/master.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/channel/channel_factory.h"
#include "felicia/core/master/heart_beat_listener.h"

namespace felicia {

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

Master::Master() : thread_(std::make_unique<::base::Thread>("Master")) {}

void Master::Run() {
  thread_->StartWithOptions(
      ::base::Thread::Options{::base::MessageLoop::TYPE_IO, 0});
}

void Master::Stop() { thread_->Stop(); }

void Master::RegisterClient(const RegisterClientRequest* arg,
                            RegisterClientResponse* result,
                            StatusCallback callback) {
  ClientInfo client_info = arg->client_info();
  std::unique_ptr<Client> client = Client::NewClient(client_info);
  if (!client) {
    std::move(callback).Run(errors::FailedToRegisterClient());
  }

  uint32_t id = client->client_info().id();
  result->set_id(id);
  AddClient(id, std::move(client));
  DLOG(INFO) << "[RegisterClient]: " << ::base::StringPrintf("client(%d)", id);
  std::move(callback).Run(Status::OK());

  thread_->task_runner()->PostTask(
      FROM_HERE, ::base::BindOnce(&Master::DoCheckHeart,
                                  ::base::Unretained(this), client_info));
}

void Master::RegisterNode(const RegisterNodeRequest* arg,
                          RegisterNodeResponse* result,
                          StatusCallback callback) {
  NodeInfo node_info = arg->node_info();
  if (!CheckIfClientExists(node_info.client_id())) {
    std::move(callback).Run(errors::ClientNotRegistered());
    return;
  }

  std::unique_ptr<Node> node = Node::NewNode(node_info);
  if (!node) {
    std::move(callback).Run(errors::NodeAlreadyRegistered(node_info));
    return;
  }

  *result->mutable_node_info() = node->node_info();
  DLOG(INFO) << "[RegisterNode]: "
             << ::base::StringPrintf("node(%s)",
                                     node->node_info().name().c_str());
  AddNode(std::move(node));
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

void Master::ListNodes(const ListNodesRequest* arg, ListNodesResponse* result,
                       StatusCallback callback) {
  const NodeFilter& node_filter = arg->node_filter();
  std::vector<::base::WeakPtr<Node>> nodes = FindNodes(node_filter);
  if (!node_filter.name().empty()) {
    auto pub_sub_topics = result->mutable_pub_sub_topics();
    ::base::AutoLock l(lock_);
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
    ::base::AutoLock l(lock_);
    for (auto node : nodes) {
      if (node) *result->add_node_infos() = node->node_info();
    }
  }
  DLOG(INFO) << "[ListNodes]";
  std::move(callback).Run(Status::OK());
}

void Master::PublishTopic(const PublishTopicRequest* arg,
                          PublishTopicResponse* result,
                          StatusCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const TopicInfo& topic_info = arg->topic_info();
  NodeFilter node_filter;
  node_filter.set_publishing_topic(topic_info.topic());
  std::vector<::base::WeakPtr<Node>> publishing_nodes = FindNodes(node_filter);
  Reason reason;
  if (publishing_nodes.size() > 0) {
    reason = Reason::TopicAlreadyPublishing;
  } else {
    ::base::WeakPtr<Node> node = FindNode(node_info);
    ::base::AutoLock l(lock_);
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
    std::move(callback).Run(Status::OK());
    DLOG(INFO) << "[PublishTopic]: "
               << ::base::StringPrintf("topic(%s) from node(%s)",
                                       topic_info.topic().c_str(),
                                       node_info.name().c_str());
  } else if (reason == Reason::TopicAlreadyPublishing) {
    std::move(callback).Run(errors::TopicAlreadyPublishing(topic_info));
    return;
  } else if (reason == Reason::UnknownFailed) {
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
  ::base::WeakPtr<Node> node = FindNode(node_info);
  Reason reason;
  {
    ::base::AutoLock l(lock_);
    if (node) {
      if (!node->IsPublishingTopic(topic)) {
        reason = Reason::TopicNotPublishingOnNode;
      } else {
        node->UnregisterPublishingTopic(topic);
        reason = Reason::None;
      }
    } else {
      reason = Reason::UnknownFailed;
    }
  }

  if (reason == Reason::None) {
    std::move(callback).Run(Status::OK());
    DLOG(INFO) << "[UnpublishTopic]: "
               << ::base::StringPrintf("topic(%s) from node(%s)", topic.c_str(),
                                       node_info.name().c_str());
  } else if (reason == Reason::TopicNotPublishingOnNode) {
    std::move(callback).Run(errors::TopicNotPublishingOnNode(node_info, topic));
  } else if (reason == Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToUnpublish(topic));
  }
}

void Master::SubscribeTopic(const SubscribeTopicRequest* arg,
                            SubscribeTopicResponse* result,
                            StatusCallback callback) {
  const NodeInfo& node_info = arg->node_info();
  CHECK_NODE_EXISTS(node_info);

  const std::string& topic = arg->topic();
  ::base::WeakPtr<Node> node = FindNode(node_info);
  Reason reason;
  {
    ::base::AutoLock l(lock_);
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
    std::move(callback).Run(Status::OK());
    DLOG(INFO) << "[SubscribeTopic]: "
               << ::base::StringPrintf("topic(%s) from node(%s)", topic.c_str(),
                                       node_info.name().c_str());
  } else if (reason == Reason::TopicNotPublishingOnNode) {
    std::move(callback).Run(
        errors::TopicAlreadySubscribingOnNode(node_info, topic));
    return;
  } else if (reason == Reason::UnknownFailed) {
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
  ::base::WeakPtr<Node> node = FindNode(node_info);
  Reason reason;
  {
    ::base::AutoLock l(lock_);
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
    std::move(callback).Run(Status::OK());
    DLOG(INFO) << "[UnsubscribeTopic]: "
               << ::base::StringPrintf("topic(%s) from node(%s)", topic.c_str(),
                                       node_info.name().c_str());
  } else if (reason == Reason::TopicNotSubscribingOnNode) {
    std::move(callback).Run(
        errors::TopicNotSubscribingOnNode(node_info, topic));
  } else if (reason == Reason::UnknownFailed) {
    std::move(callback).Run(errors::FailedToUnsubscribe(topic));
  }
}

void Master::ListTopics(const ListTopicsRequest* arg,
                        ListTopicsResponse* result, StatusCallback callback) {
  const TopicFilter& topic_filter = arg->topic_filter();
  std::vector<TopicInfo> topic_infos = FindTopicInfos(topic_filter);
  for (auto& topic_info : topic_infos) {
    *result->add_topic_infos() = topic_info;
  }
  DLOG(INFO) << "[ListTopics]";
  std::move(callback).Run(Status::OK());
}

void Master::Gc() { LOG(ERROR) << "Not implemented"; }

::base::WeakPtr<Node> Master::FindNode(const NodeInfo& node_info) {
  ::base::AutoLock l(lock_);
  auto it = client_map_.find(node_info.client_id());
  if (it == client_map_.end()) return nullptr;
  return it->second->FindNode(node_info);
}

std::vector<::base::WeakPtr<Node>> Master::FindNodes(
    const NodeFilter& node_filter) {
  ::base::AutoLock l(lock_);
  std::vector<::base::WeakPtr<Node>> nodes;
  auto it = client_map_.begin();
  if (!node_filter.publishing_topic().empty()) {
    while (it != client_map_.end()) {
      std::vector<::base::WeakPtr<Node>> tmp_nodes =
          it->second->FindNodes(node_filter);
      nodes.insert(nodes.end(), tmp_nodes.begin(), tmp_nodes.end());
      // Because there can be only one publishing node.
      if (nodes.size() > 0) return nodes;
      it++;
    }
  } else {
    while (it != client_map_.end()) {
      std::vector<::base::WeakPtr<Node>> tmp_nodes =
          it->second->FindNodes(node_filter);
      nodes.insert(nodes.end(), tmp_nodes.begin(), tmp_nodes.end());
      it++;
    }
  }
  return nodes;
}

std::vector<TopicInfo> Master::FindTopicInfos(const TopicFilter& topic_filter) {
  ::base::AutoLock l(lock_);
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

void Master::AddClient(uint32_t id, std::unique_ptr<Client> client) {
  DLOG(INFO) << "Master::AddClient() " << client->client_info().DebugString();
  ::base::AutoLock l(lock_);
  client_map_.insert_or_assign(id, std::move(client));
}

void Master::RemoveClient(const ClientInfo& client_info) {
  DLOG(INFO) << "Master::RemoveClient() " << client_info.DebugString();
  ::base::AutoLock l(lock_);
  client_map_.erase(client_map_.find(client_info.id()));
}

void Master::AddNode(std::unique_ptr<Node> node) {
  DLOG(INFO) << "Master::AddNode() " << node->node_info().DebugString();
  uint32_t id = node->node_info().client_id();
  ::base::AutoLock l(lock_);
  client_map_[id]->AddNode(std::move(node));
}

void Master::RemoveNode(const NodeInfo& node_info) {
  DLOG(INFO) << "Master::RemoveNode() " << node_info.DebugString();
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
  std::vector<::base::WeakPtr<Node>> publishing_nodes = FindNodes(node_filter);
  if (publishing_nodes.size() > 0) {
    ::base::WeakPtr<Node> publishing_node = publishing_nodes[0];
    {
      ::base::AutoLock l(lock_);
      if (publishing_node)
        DoNotifySubscriber(subscribing_node_info,
                           publishing_node->GetTopicInfo(topic));
    }
  }
}

void Master::NotifyAllSubscribers(const TopicInfo& topic_info) {
  NodeFilter node_filter;
  node_filter.set_subscribing_topic(topic_info.topic());
  std::vector<::base::WeakPtr<Node>> subscribing_nodes = FindNodes(node_filter);
  {
    ::base::AutoLock l(lock_);
    for (auto& subscribing_node : subscribing_nodes) {
      if (subscribing_node)
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

#undef CHECK_CLIENT_EXISTS
#undef CHECK_NODE_EXISTS

}  // namespace felicia