#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_UTIL_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_UTIL_H_

#include "felicia/drivers/camera/camera_format.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/size.h"

namespace felicia {

EXPORT size_t AllocationSize(CameraFormat camera_format);

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_UTIL_H_