#include "felicia/core/util/timestamp/timestamper.h"

namespace felicia {

Timestamper::Timestamper() = default;

Timestamper::~Timestamper() = default;

base::TimeDelta Timestamper::timestamp() {
  const base::TimeTicks now = base::TimeTicks::Now();
  if (base_time_ref_.is_null()) {
    base_time_ref_ = now;
  }

  return now - base_time_ref_;
}

ThreadSafeTimestamper::ThreadSafeTimestamper() = default;

ThreadSafeTimestamper::~ThreadSafeTimestamper() = default;

base::TimeDelta ThreadSafeTimestamper::timestamp() {
  int64_t placeholder_zero = 0;
  const base::TimeTicks now = base::TimeTicks::Now();
  const int64_t now_in_ms = (now - base::TimeTicks()).InMicroseconds();
  base_time_in_ms_.compare_exchange_strong(placeholder_zero, now_in_ms);

  return base::TimeDelta::FromMicroseconds(
      now_in_ms - base_time_in_ms_.load(std::memory_order_acquire));
}

}  // namespace felicia