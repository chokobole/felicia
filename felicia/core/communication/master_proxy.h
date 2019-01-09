#ifndef FELICIA_CORE_COMMUNICATION_MASTER_PROXY_H_
#define FELICIA_CORE_COMMUNICATION_MASTER_PROXY_H_

#include "absl/memory/memory.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/no_destructor.h"
#include "felicia/core/lib/message_loop/message_loop.h"
#include "felicia/core/node/node.h"

namespace felicia {

class EXPORT MasterProxy {
 public:
  static MasterProxy& GetInstance();

  template <typename NodeTy>
  void RequestCreateNode(const NodeInfo& node_info);
  void Spin();

 private:
  friend class NoDestructor<MasterProxy>;
  MasterProxy();

  std::unique_ptr<felicia::MessageLoop> message_loop_;
  std::vector<Node*> nodes_;
};

template <typename NodeTy>
void MasterProxy::RequestCreateNode(const NodeInfo& node_info) {
  Node* node = new NodeTy();
  node->set_node_info(node_info);
  node->OnInit();
  node->OnDidCreate();
  nodes_.push_back(node);
}

}  // namespace felicia

#endif  // FELICIA_CORE_COMMUNICATION_MASTER_PROXY_H_