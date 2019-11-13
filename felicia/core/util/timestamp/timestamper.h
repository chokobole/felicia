// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_UTIL_TIMESTAMP_TIMESTAMPER_H_
#define FELICIA_CORE_UTIL_TIMESTAMP_TIMESTAMPER_H_

#include <atomic>

#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

// Used by single thread task which needs timestamp.
class FEL_EXPORT Timestamper {
 public:
  Timestamper();
  ~Timestamper();

  base::TimeDelta timestamp();

 protected:
  base::TimeTicks base_time_ref_;
};

// Used by multi thread task which needs timestamp. It ensures only one thread
// set |base_time_in_ms_|.
class FEL_EXPORT ThreadSafeTimestamper {
 public:
  ThreadSafeTimestamper();
  ~ThreadSafeTimestamper();

  base::TimeDelta timestamp();

 private:
  std::atomic<int64_t> base_time_in_ms_{0};
};

}  // namespace felicia

#endif  // FELICIA_CORE_UTIL_TIMESTAMP_TIMESTAMPER_H_