#include "felicia/core/lib/unit/time_util.h"

namespace felicia {

#if defined(HAS_ROS)
ros::Time ToRosTime(base::TimeDelta time) {
  return ros::Time{time.InSecondsF()};
}

base::TimeDelta FromRosTime(ros::Time time) {
  return base::TimeDelta::FromNanoseconds(static_cast<int64_t>(time.toNSec()));
}
#endif  // defined(HAS_ROS)

}  // namespace felicia