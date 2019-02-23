#include "felicia/core/master/master.h"

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/base/rand_util.h"
#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/platform/net_util.h"

namespace felicia {

Master::Master() : thread_(std::make_unique<::base::Thread>("Main")) {
  IPEndPoint* self_ip_endpoint = master_node_info_.mutable_ip_endpoint();
  self_ip_endpoint->set_ip(net::HostIPAddress(net::HOST_IP_ONLY_ALLOW_IPV4));
}

void Master::Run() {
  thread_->StartWithOptions(
      ::base::Thread::Options{::base::MessageLoop::TYPE_IO, 0});
}

void Master::Stop() { thread_->Stop(); }

#define IF_NO_NODE_RETURN()                                               \
  NodeInfo node_info = arg->node_info();                                  \
  ServerNode* self_node = FindNode(node_info);                            \
  do {                                                                    \
    if (!self_node) {                                                     \
      callback(errors::NotFound(::base::StringPrintf(                     \
          "Node '%s' isn't registered yet.", node_info.name().c_str()))); \
      return;                                                             \
    }                                                                     \
  } while (false)

bool NodeNameChecker::operator()(const ServerNode& node) {
  return strings::Equals(node.node_info().name(), node_info_.name());
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

  AddNode(node_info);
  *result->mutable_node_info() = node_info;
  LOG(INFO) << "[RegisterNode]: "
            << ::base::StringPrintf("node(%s)", node_info.name().c_str());
  callback(Status::OK());
}

void Master::UnregisterNode(const UnregisterNodeRequest* arg,
                            UnregisterNodeResponse* result,
                            StatusCallback callback) {
  IF_NO_NODE_RETURN();

  RemoveNode(node_info);
  LOG(INFO) << "[UnregisterNode]: "
            << ::base::StringPrintf("node(%s)", node_info.name().c_str());
  callback(Status::OK());
}

void Master::GetNodes(const GetNodesRequest* arg, GetNodesResponse* result,
                      StatusCallback callback) {
  const NodeFilter& node_filter = arg->node_filter();
  std::vector<ServerNode*> nodes = FindNode(node_filter);
  for (auto node : nodes) {
    *result->add_node_infos() = node->node_info();
  }
  LOG(INFO) << "[GetNodes]";
  callback(Status::OK());
}

void Master::PublishTopic(const PublishTopicRequest* arg,
                          PublishTopicResponse* result,
                          StatusCallback callback) {
  const TopicSource& topic_source = arg->topic_source();
  ServerNode* self_node = FindNode(topic_source.node_info());
  if (!self_node) {
    callback(errors::NotFound(::base::StringPrintf(
        "Node '%s' isn't registered yet.", self_node->name().data())));
    return;
  }

  ::base::StringPiece topic = topic_source.topic();
  NodeFilter node_filter;
  node_filter.set_publishing_topic(std::string(topic));
  std::vector<ServerNode*> publishing_nodes = FindNode(node_filter);
  if (publishing_nodes.size() > 0) {
    callback(errors::AlreadyExists(::base::StringPrintf(
        "Topic '%s' is already being publishied.", topic.data())));
    return;
  }
  LOG(INFO) << "[PublishTopic]: "
            << ::base::StringPrintf("topic(%s) from node(%s)", topic.data(),
                                    self_node->name().data());
  self_node->RegisterPublishingTopic(topic, topic_source);
  callback(Status::OK());

  node_filter.Clear();
  node_filter.set_subscribing_topic(std::string(topic));
}

void Master::UnpublishTopic(const UnpublishTopicRequest* arg,
                            UnpublishTopicResponse* result,
                            StatusCallback callback) {
  IF_NO_NODE_RETURN();

  ::base::StringPiece topic = arg->topic();
  if (!self_node->IsPublishingTopic(topic)) {
    callback(errors::NotFound(
        ::base::StringPrintf("Node '%s' isn't publishing topic '%s'.",
                             self_node->name().data(), topic.data())));
    return;
  }
  LOG(INFO) << "[UnpublishTopic]: "
            << ::base::StringPrintf("topic(%s) from node(%s)", topic.data(),
                                    self_node->name().data());
  self_node->UnregisterPublishingTopic(topic);
  callback(Status::OK());
}

void Master::SubscribeTopic(const SubscribeTopicRequest* arg,
                            SubscribeTopicResponse* result,
                            StatusCallback callback) {
  IF_NO_NODE_RETURN();

  ::base::StringPiece topic = arg->topic();
  if (self_node->IsSubsribingTopic(topic)) {
    callback(errors::AlreadyExists(
        ::base::StringPrintf("Node '%s' is already subscribing topic '%s'.",
                             self_node->name().data(), topic.data())));
    return;
  }
  LOG(INFO) << "[SubscribeTopic]: "
            << ::base::StringPrintf("topic(%s) from node(%s)", topic.data(),
                                    self_node->name().data());
  self_node->RegisterSubscribingTopic(topic);
  callback(Status::OK());

  thread_->task_runner()->PostTask(
      FROM_HERE,
      ::base::Bind(&Master::NotifyTopicSource, ::base::Unretained(this),
                   std::string(topic), node_info));
}

void Master::UnsubscribeTopic(const UnsubscribeTopicRequest* arg,
                              UnsubscribeTopicResponse* result,
                              StatusCallback callback) {
  IF_NO_NODE_RETURN();

  ::base::StringPiece topic = arg->topic();
  if (!self_node->IsSubsribingTopic(topic)) {
    callback(errors::AlreadyExists(
        ::base::StringPrintf("Node '%s' isn't subscribing topic '%s'.",
                             self_node->name().data(), topic.data())));
    return;
  }
  LOG(INFO) << "[UnsubscribeTopic]: "
            << ::base::StringPrintf("topic(%s) from node(%s)", topic.data(),
                                    self_node->name().data());
  self_node->UnregisterSubscribingTopic(topic);
  callback(Status::OK());
}

void Master::Gc() { LOG(ERROR) << "Not implemented"; }

ServerNode* Master::FindNode(const NodeInfo& node_info) {
  ::base::StringPiece name = node_info.name();
  for (auto& node : nodes_) {
    if (strings::Equals(node.name(), name)) {
      return &node;
    }
  }

  return nullptr;
}

std::vector<ServerNode*> Master::FindNode(const NodeFilter& node_filter) {
  std::vector<ServerNode*> nodes;
  if (node_filter.all()) {
    for (auto& node : nodes_) {
      nodes.push_back(&node);
    }
  } else if (!node_filter.publishing_topic().empty()) {
    for (auto& node : nodes_) {
      if (node.IsPublishingTopic(node_filter.publishing_topic())) {
        nodes.push_back(&node);
#if !DCHECK_IS_ON()
        break;
#endif
      }
    }
    // Publishing node should be only one.
    DCHECK(nodes.size() == 0 || nodes.size() == 1);
  } else if (!node_filter.subscribing_topic().empty()) {
    for (auto& node : nodes_) {
      if (node.IsSubsribingTopic(node_filter.subscribing_topic())) {
        nodes.push_back(&node);
      }
    }
  }

  return nodes;
}

void Master::AddNode(const NodeInfo& node_info) {
  nodes_.emplace_back(node_info);
}

void Master::RemoveNode(const NodeInfo& node_info) {
  nodes_.erase(
      std::remove_if(nodes_.begin(), nodes_.end(), NodeNameChecker{node_info}),
      nodes_.end());
}

void Master::NotifyTopicSource(std::string topic,
                               const NodeInfo& subscribing_node_info) {
  NodeFilter node_filter;
  node_filter.set_publishing_topic(std::string(topic));
  std::vector<ServerNode*> publishing_nodes = FindNode(node_filter);
  if (publishing_nodes.size() > 0) {
    ServerNode* publishing_node = publishing_nodes[0];

    auto channel = ChannelFactory::NewChannel<TopicSource>(ChannelDef{});
    // publishing_node->AddChannel(std::move(channel));

    LOG(INFO) << "Connect To: " << subscribing_node_info.DebugString();
    Channel<TopicSource>* raw_channel = channel.get();
    TopicSource source = publishing_node->GetTopicSource(topic);
    channel->Connect(
        subscribing_node_info, [raw_channel, source](const Status& s) {
          if (s.ok()) {
            LOG(INFO) << "Success to connect topic source channel";
            raw_channel->SendMessage(source, [](const Status& s) {
              if (s.ok()) {
                LOG(INFO) << "Success to send message";
              } else {
                LOG(ERROR) << "Failed to send message: " << s.error_message();
              }
            });
          } else {
            LOG(ERROR) << "Failed to connect topic source channel: "
                       << s.error_message();
          }
        });
    topic_source_channels_.push_back(std::move(channel));
  }
}

}  // namespace felicia