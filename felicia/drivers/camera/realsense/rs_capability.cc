#include "felicia/drivers/camera/realsense/rs_capability.h"

#include "third_party/chromium/base/logging.h"

namespace felicia {

const RsCapability& GetBestMatchedCapability(
    const CameraFormat& requested, const RsCapabilityList& capabilities) {
  DCHECK(!capabilities.empty());
  const RsCapability* best_match = &(*capabilities.begin());
  for (const RsCapability& capability : capabilities) {
    if (CompareCapability(requested, capability.supported_format,
                          best_match->supported_format)) {
      best_match = &capability;
    }
  }
  return *best_match;
}

}  // namespace felicia