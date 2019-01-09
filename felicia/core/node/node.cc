#include "felicia/core/node/node.h"

#include <algorithm>

#include "felicia/core/communication/master_proxy.h"

namespace felicia {

NodeInfo::NodeInfo() = default;

NodeInfo::NodeInfo(std::string name, const net::IPEndPoint& ip_endpoint)
    : name_(name), ip_endpoint_(ip_endpoint) {}

NodeInfo::NodeInfo(const NodeInfo& node_info) = default;

NodeInfo::~NodeInfo() = default;

Node::Node() : master_proxy_(nullptr) {}

Node::~Node() = default;

MasterProxy* Node::master_proxy() const { return master_proxy_; }
void Node::set_master_proxy(MasterProxy* master_proxy) {
  master_proxy_ = master_proxy;
}

void Node::PublishMessage(const std::string& topic_name,
                          const MessageCallback& message_callback) {
  if (IsRegisteredTopicName(topic_name)) {
    LOG(WARNING) << "Already registered topic_name with " << topic_name;
    // TODO(chokobole)
    // OnError();
    return;
  }

  auto connection = absl::make_unique<TCPConnection>();
  int rv = connection->Setup();
  if (rv != net::OK) {
    OnError(rv);
    return;
  }

  connections_[topic_name] = std::move(connection);
  RegisterTopicName(topic_name);
}

bool Node::IsRegisteredTopicName(const std::string& topic_name) {
  return false;
}

void Node::RegisterTopicName(const std::string& topic_name) {}

void Node::EnqueMessage(const std::string& topic_name,
                        const std::string& message) {
  default_message_.Clear();
  const Message new_message = default_message_.ParseFromString(message);
  message_queue_.EnqueMessage(topic_name, new_message);
}

Message& Node::PopMessage(const std::string& topic_name) {
  return message_queue_.PopMessage(topic_name);
}

void Node::HandleMessages() {}

}  // namespace felicia
