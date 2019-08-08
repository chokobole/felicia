#include "felicia/drivers/vendors/zed/zed_capability.h"

#include "third_party/chromium/base/logging.h"

namespace felicia {

CameraFormat ConvertToCameraFormat(const ZedCapability& capability) {
  CameraFormat camera_format;
  camera_format.set_pixel_format(PixelFormat::PIXEL_FORMAT_BGRA);
  camera_format.set_frame_rate(capability.frame_rate);

  switch (capability.resolution) {
    case sl::RESOLUTION_HD2K:
      camera_format.SetSize(2208, 1242);
      break;
    case sl::RESOLUTION_HD1080:
      camera_format.SetSize(1920, 1080);
      break;
    case sl::RESOLUTION_HD720:
      camera_format.SetSize(1280, 720);
      break;
    case sl::RESOLUTION_VGA:
      camera_format.SetSize(672, 376);
      break;
    default:
      NOTREACHED() << "Invalid resolution";
  }

  return camera_format;
}

const ZedCapability* GetBestMatchedCapability(const CameraFormat& requested) {
  const ZedCapability* best_match = nullptr;
  for (auto& capability : kZedCapabilities) {
    if (!best_match) {
      best_match = &capability;
    } else {
      if (CompareCapability(requested, ConvertToCameraFormat(capability),
                            ConvertToCameraFormat(*best_match))) {
        best_match = &capability;
      }
    }
  }
  return best_match;
}

}  // namespace felicia