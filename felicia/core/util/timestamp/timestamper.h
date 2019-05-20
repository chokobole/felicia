#ifndef FELICIA_CORE_UTIL_TIMESTAMP_TIMESTAMPER_H_
#define FELICIA_CORE_UTIL_TIMESTAMP_TIMESTAMPER_H_

#include <atomic>

#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

// Used by single thread task which needs timestamp.
class EXPORT Timestamper {
 public:
  virtual ~Timestamper();

  ::base::TimeDelta timestamp();

 protected:
  ::base::TimeTicks base_time_ref_;
};

// Used by multi thread task which needs timestamp. It ensures only one thread
// set |base_time_in_ms_|.
class EXPORT ThreadSafeTimestamper {
 public:
  ::base::TimeDelta timestamp();

 private:
  std::atomic<int64_t> base_time_in_ms_{0};
};

}  // namespace felicia

#endif  // FELICIA_CORE_UTIL_TIMESTAMP_TIMESTAMPER_H_