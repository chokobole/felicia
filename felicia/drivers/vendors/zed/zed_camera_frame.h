#ifndef FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FRAME_H_
#define FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FRAME_H_

#include <sl_core/utils/Core.hpp>

#include "felicia/drivers/camera/camera_frame.h"

namespace felicia {

CameraFrame ConverToCameraFrame(::sl::Mat image,
                                const CameraFormat& camera_format);

}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FRAME_H_