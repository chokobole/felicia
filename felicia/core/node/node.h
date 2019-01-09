#ifndef FELICIA_CORE_NODE_NODE_H_
#define FELICIA_CORE_NODE_NODE_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "absl/memory/memory.h"

#include "felicia/core/communication/connection.h"
#include "felicia/core/communication/message_queue.h"
#include "felicia/core/communication/tcp_connection.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/macros.h"
#include "felicia/core/lib/net/ip_endpoint.h"

namespace felicia {

using MessageCallback = std::function<Message(void)>;

class MasterProxy;
class EXPORT NodeInfo {
 public:
  NodeInfo();
  NodeInfo(std::string name, const net::IPEndPoint& ip_endpoint);
  NodeInfo(const NodeInfo& node_info);
  ~NodeInfo();

  const std::string& name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }
  const net::IPEndPoint& ip_endpoint() const { return ip_endpoint_; }
  void set_ip_endpoint(const net::IPEndPoint& ip_endpoint) {
    ip_endpoint_ = ip_endpoint;
  }

 private:
  std::string name_;
  net::IPEndPoint ip_endpoint_;
};

const uint8_t kDefaultQueueSize = 8;

class EXPORT Node {
 public:
  // Life cycle functions
  // It is called before created, each node has to initialize.
  virtual void OnInit() {}
  // It is called when succeeded to be created.
  virtual void OnDidCreate() {}
  // It is called when message is arrived.
  virtual void OnMessage(const std::string& topic_name,
                         const Message& message) {}
  // It is called when error happens
  virtual void OnError(int rv) {}

  template <typename T>
  void Subscribe(const std::string& topic_name,
                 uint8_t size = kDefaultQueueSize);
  void PublishMessage(const std::string& topic_name,
                      const MessageCallback& message_callback);

 protected:
  Node();
  virtual ~Node();

 private:
  friend class MasterProxy;

  const NodeInfo& node_info() const { return node_info_; }
  void set_node_info(const NodeInfo& node_info) { node_info_ = node_info; }
  MessageQueue& message_queue() { return message_queue_; }
  MasterProxy* master_proxy() const;
  void set_master_proxy(MasterProxy* master_proxy);

  bool IsRegisteredTopicName(const std::string& topic_name);
  void RegisterTopicName(const std::string& topic_name);

  void EnqueMessage(const std::string& topic_name, const std::string& message);
  Message& PopMessage(const std::string& topic_name);
  // Check if there are messages to handle and call OnMessage method.
  void HandleMessages();

  NodeInfo node_info_;
  MessageQueue message_queue_;
  Message default_message_;
  MasterProxy* master_proxy_;

  std::unordered_map<std::string, std::unique_ptr<Connection>> connections_;

  DISALLOW_COPY_AND_ASSIGN(Node);
};

template <typename T>
void Node::Subscribe(const std::string& topic_name, uint8_t queue_size) {
  // Tell MasterProxy there is a node that wants to subscribe topic_name
  auto connection = absl::make_unique<TCPConnection>();
  int rv = connection->Connect();
  if (rv == net::OK) connections_[topic_name] = std::move(connection);

  message_queue_.AllocateQueue(topic_name, queue_size);
  default_message_ = Message(T());
}

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_NODE_H_
