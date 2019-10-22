#ifndef FELICIA_CORE_NODE_LIFECYCLE_H_
#define FELICIA_CORE_NODE_LIFECYCLE_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace felicia {

class EXPORT NodeLifecycle {
 public:
  NodeLifecycle();
  virtual ~NodeLifecycle();

  // Life cycle functions
  // It is called before created, each node has to initialize.
  virtual void OnInit();
  // It is called when succeeded to be created.
  virtual void OnDidCreate(NodeInfo node_info);
  // It is called when error happens
  virtual void OnError(Status status);

  DISALLOW_COPY_AND_ASSIGN(NodeLifecycle);
};

}  // namespace felicia

#endif  // FELICIA_CORE_NODE_LIFECYCLE_H_