#ifndef FELICIA_DRIVERS_VENDORS_REALSENSE_RS_CAPABILITY_H_
#define FELICIA_DRIVERS_VENDORS_REALSENSE_RS_CAPABILITY_H_

#include <list>

#include "third_party/chromium/base/containers/flat_map.h"

#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/vendors/realsense/rs_stream_info.h"

namespace felicia {

struct RsCapability {
  RsCapability(int stream_index, const CameraFormat& format)
      : supported_format(format), stream_index(stream_index) {}

  const CameraFormat supported_format;
  const int stream_index;
};

using RsCapabilityList = std::list<RsCapability>;
using RsCapabilityMap = ::base::flat_map<RsStreamInfo, RsCapabilityList>;

const RsCapability& GetBestMatchedCapability(
    const CameraFormat& requested, const RsCapabilityList& capabilities);

}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_REALSENSE_RS_CAPABILITY_H_