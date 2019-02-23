#ifndef FELICIA_CC_CLIENT_NODE_H_
#define FELICIA_CC_CLIENT_NODE_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/master/master_data.pb.h"
#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

class EXPORT ClientNode : public NodeLifecycle {
 public:
  explicit ClientNode(const NodeInfo& node_info) : node_info_(node_info) {}

  const NodeInfo& node_info() { return node_info_; }

 private:
  NodeInfo node_info_;

  DISALLOW_COPY_AND_ASSIGN(ClientNode);
};

}  // namespace felicia
#endif  // FELICIA_CC_CLIENT_NODE_H_