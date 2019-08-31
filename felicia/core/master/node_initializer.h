#ifndef FELICIA_CORE_MASTER_NODE_INITIALIZER_H_
#define FELICIA_CORE_MASTER_NODE_INITIALIZER_H_

#include <memory>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/core/node/node_lifecycle.h"

namespace felicia {

// NodeLifecycle::OnInit() sometimes takes long, and in order to keep sending
// heart beat, OnInit() should be executed on a different thread.
class NodeInitializer {
 public:
  typedef base::OnceCallback<void(std::unique_ptr<NodeLifecycle>)>
      OnInitCallback;

  explicit NodeInitializer(std::unique_ptr<NodeLifecycle> node_lifecycle);

  void Initialize(OnInitCallback callback);

 private:
  void DoInitialize();
  void ReleaseSelf();

  std::unique_ptr<NodeLifecycle> node_lifecycle_;
  base::Thread thread_;
  OnInitCallback callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_NODE_INITIALIZER_H_
