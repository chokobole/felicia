#ifndef FELICIA_CORE_MASTER_CLIENT_CLIENT_H_
#define FELICIA_CORE_MASTER_CLIENT_CLIENT_H_

#include <vector>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/threading/thread_collision_warner.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/node/node.h"

namespace felicia {

class EXPORT Client {
 public:
  Client() = default;

  explicit Client(const ClientInfo& client_info) : client_info_(client_info) {}

  const ClientInfo& client_info() const { return client_info_; }

  // Add the Node constructed from a given |node_info|.
  void AddNode(const NodeInfo& node_info);
  // Remove the Node whose |node_info| is same with a given |node_info|.
  void RemoveNode(const NodeInfo& node_info);
  // Check there has a Node whose |node_info| is same with a given |node_info|.
  bool HasNode(const NodeInfo& node_info) const;
  // Find the node whose |node_info| is same with a given |node_info|.
  Node* FindNode(const NodeInfo& node_info);
  // Find the nodes which meets the given condition |node_filter|.
  std::vector<Node*> FindNodes(const NodeFilter& node_filter);

 private:
  ClientInfo client_info_;
  std::vector<Node> nodes_;
  DFAKE_MUTEX(add_remove_);
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_CLIENT_CLIENT_H_