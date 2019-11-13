// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_UTIL_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_UTIL_H_

#include "felicia/drivers/camera/camera_format.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/size.h"

namespace felicia {
namespace drivers {
namespace camera_internal {

FEL_EXPORT size_t AllocationSize(const CameraFormat& camera_format);

}  // namespace camera_internal
}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_UTIL_H_