// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_UNIT_TIME_UTIL_H_
#define FELICIA_CORE_LIB_UNIT_TIME_UTIL_H_

#if defined(HAS_ROS)
#include <ros/time.h>
#endif  // defined(HAS_ROS)

#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

#if defined(HAS_ROS)
FEL_EXPORT ros::Time ToRosTime(base::TimeDelta time);
FEL_EXPORT base::TimeDelta FromRosTime(ros::Time time);
#endif  // defined(HAS_ROS)

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_TIME_UTIL_H_