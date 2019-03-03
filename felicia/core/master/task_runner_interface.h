#ifndef FELICIA_CORE_MASTER_TASK_RUNNER_INTERFACE_H_
#define FELICIA_CORE_MASTER_TASK_RUNNER_INTERFACE_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/location.h"

namespace felicia {

class TaskRunnerInterface {
 public:
  virtual ~TaskRunnerInterface() = default;

  virtual bool PostTask(const ::base::Location& from_here,
                        ::base::OnceClosure callback) = 0;

  virtual bool PostDelayedTask(const ::base::Location& from_here,
                               ::base::OnceClosure callback,
                               ::base::TimeDelta delay) = 0;
};

}  // namespace felicia

#endif  // FELICIA_CORE_MASTER_TASK_RUNNER_INTERFACE_H_