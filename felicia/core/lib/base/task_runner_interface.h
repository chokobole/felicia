#ifndef FELICIA_CORE_LIB_BASE_TASK_RUNNER_INTERFACE_H_
#define FELICIA_CORE_LIB_BASE_TASK_RUNNER_INTERFACE_H_

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/location.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

class EXPORT TaskRunnerInterface {
 public:
  TaskRunnerInterface();
  virtual ~TaskRunnerInterface();

  virtual bool IsBoundToCurrentThread() const = 0;

  virtual bool PostTask(const base::Location& from_here,
                        base::OnceClosure callback) = 0;

  virtual bool PostDelayedTask(const base::Location& from_here,
                               base::OnceClosure callback,
                               base::TimeDelta delay) = 0;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_BASE_TASK_RUNNER_INTERFACE_H_