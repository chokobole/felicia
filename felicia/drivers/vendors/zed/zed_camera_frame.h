#ifndef FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FRAME_H_
#define FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FRAME_H_

#include <sl_core/utils/Core.hpp>

#include "felicia/drivers/camera/depth_camera_frame.h"

namespace felicia {

CameraFrame ConvertToCameraFrame(::sl::Mat image,
                                 const CameraFormat& camera_format);

DepthCameraFrame ConvertToDepthCameraFrame(::sl::Mat image,
                                           const CameraFormat& camera_format,
                                           float min, float max);

}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_ZED_ZED_CAMERA_FRAME_H_