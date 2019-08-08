// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_DRIVER_CAMERA_TIMESTAMP_CONSTANTS_H_
#define FELICIA_DRIVER_CAMERA_TIMESTAMP_CONSTANTS_H_

#include <stdint.h>

#include <limits>

#include "third_party/chromium/base/time/time.h"

namespace felicia {

// Indicates an invalid or missing timestamp.
constexpr base::TimeDelta kNoTimestamp =
    base::TimeDelta::FromMicroseconds(std::numeric_limits<int64_t>::min());

// Represents an infinite stream duration.
constexpr base::TimeDelta kInfiniteDuration =
    base::TimeDelta::FromMicroseconds(std::numeric_limits<int64_t>::max());

}  // namespace felicia

#endif  // FELICIA_DRIVER_CAMERA_TIMESTAMP_CONSTANTS_H_
