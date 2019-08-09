#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_UTIL_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_UTIL_H_

#include "felicia/drivers/camera/camera_format.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/size.h"

namespace felicia {
namespace camera_internal {

EXPORT size_t AllocationSize(const CameraFormat& camera_format);

}  // namespace camera_internal
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_UTIL_H_