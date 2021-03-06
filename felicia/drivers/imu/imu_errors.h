// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_DRIVERS_IMU_IMU_ERRORS_H_
#define FELICIA_DRIVERS_IMU_IMU_ERRORS_H_

#include "felicia/core/lib/error/errors.h"

namespace felicia {
namespace drivers {
namespace errors {

inline felicia::Status NoImuCapability() {
  return felicia::errors::NotFound("No imu capability.");
}

}  // namespace errors
}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_ERRORS_H_