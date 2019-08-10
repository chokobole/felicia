#ifndef FELICIA_DRIVERS_VENDORS_REALSENSE_RS_PIXEL_FORMAT_H_
#define FELICIA_DRIVERS_VENDORS_REALSENSE_RS_PIXEL_FORMAT_H_

#include <librealsense2/rs.hpp>

#include "felicia/drivers/camera/camera_format_message.pb.h"

namespace felicia {
namespace drivers {

PixelFormat FromRs2Format(rs2_format rs2_format);
rs2_format ToRs2Format(PixelFormat pixel_format);

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_REALSENSE_RS_PIXEL_FORMAT_H_