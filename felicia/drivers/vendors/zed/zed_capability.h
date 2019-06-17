#ifndef FELICIA_DRIVERS_VENDORS_ZED_ZED_CAPABILITY_H_
#define FELICIA_DRIVERS_VENDORS_ZED_ZED_CAPABILITY_H_

#include <sl/defines.hpp>

#include "felicia/drivers/camera/camera_format.h"

namespace felicia {

struct ZedCapability {
  ::sl::RESOLUTION resolution;
  float frame_rate;
};

const ZedCapability kZedCapabilities[] = {
    {::sl::RESOLUTION_HD2K, 15},   {::sl::RESOLUTION_HD1080, 15},
    {::sl::RESOLUTION_HD1080, 30}, {::sl::RESOLUTION_HD720, 15},
    {::sl::RESOLUTION_HD720, 30},  {::sl::RESOLUTION_HD720, 60},
    {::sl::RESOLUTION_VGA, 15},    {::sl::RESOLUTION_VGA, 30},
    {::sl::RESOLUTION_VGA, 60},    {::sl::RESOLUTION_VGA, 100},
};

CameraFormat ConvertToCameraFormat(const ZedCapability& capability);

const ZedCapability* GetBestMatchedCapability(const CameraFormat& requested);

}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_ZED_ZED_CAPABILITY_H_