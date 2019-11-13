#ifndef FELICIA_CORE_MASTER_MASTER_CLIENT_INTERFACE_H_
#define FELICIA_CORE_MASTER_MASTER_CLIENT_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/master.pb.h"

namespace felicia {

class FEL_EXPORT MasterClientInterface {
 public:
  MasterClientInterface();
  virtual ~MasterClientInterface();

  virtual Status Start() = 0;
  virtual Status Stop() = 0;

#define MASTER_METHOD(Method, method, cancelable)            \
  virtual void Method##Async(const Method##Request* request, \
                             Method##Response* response,     \
                             StatusOnceCallback done) = 0;
#include "felicia/core/master/rpc/master_method_list.h"
#undef MASTER_METHOD
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_SERVICE_INTERFACE_H_