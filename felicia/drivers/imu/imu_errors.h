#ifndef FELICIA_DRIVERS_IMU_IMU_ERRORS_H_
#define FELICIA_DRIVERS_IMU_IMU_ERRORS_H_

#include "felicia/core/lib/error/errors.h"

namespace felicia {
namespace errors {

inline ::felicia::Status NoImuCapability() {
  return NotFound("No imu capability.");
}

}  // namespace errors
}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_ERRORS_H_