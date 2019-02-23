#ifndef FELICIA_CORE_NODE_LIFECYCLE_H_
#define FELICIA_CORE_NODE_LIFECYCLE_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class EXPORT NodeLifecycle {
 public:
  NodeLifecycle() = default;

  // Life cycle functions
  // It is called before created, each node has to initialize.
  virtual void OnInit() {}
  // It is called when succeeded to be created.
  virtual void OnDidCreate() {}
  // It is called when error happens
  virtual void OnError(const Status& status) {}

  DISALLOW_COPY_AND_ASSIGN(NodeLifecycle);
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_LIFECYCLE_H_