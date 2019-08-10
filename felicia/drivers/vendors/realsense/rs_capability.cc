#include "felicia/drivers/vendors/realsense/rs_capability.h"

#include "third_party/chromium/base/logging.h"

namespace felicia {
namespace drivers {

const RsCapability* GetBestMatchedCapability(
    const CameraFormat& requested, const RsCapabilityList& capabilities) {
  DCHECK(!capabilities.empty());
  const RsCapability* best_match = nullptr;
  for (const RsCapability& capability : capabilities) {
    if (capability.is_camera_format) {
      if (!best_match) {
        best_match = &capability;
      } else {
        if (CompareCapability(requested, capability.format.camera_format,
                              best_match->format.camera_format)) {
          best_match = &capability;
        }
      }
    }
  }
  return best_match;
}

const RsCapability* GetBestMatchedCapability(
    const ImuFormat& requested, const RsCapabilityList& capabilities) {
  DCHECK(!capabilities.empty());
  const RsCapability* best_match = nullptr;
  for (const RsCapability& capability : capabilities) {
    if (!capability.is_camera_format) {
      if (!best_match) {
        best_match = &capability;
      } else {
        if (CompareCapability(requested, capability.format.imu_format,
                              best_match->format.imu_format)) {
          best_match = &capability;
        }
      }
    }
  }
  return best_match;
}

}  // namespace drivers
}  // namespace felicia