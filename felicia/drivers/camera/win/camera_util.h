#ifndef FELICIA_DRIVERS_CAMERA_WIN_CAMERA_UTIL_H_
#define FELICIA_DRIVERS_CAMERA_WIN_CAMERA_UTIL_H_

#include <string>

namespace felicia {
namespace drivers {

std::string GetDeviceModelId(const std::string& device_id);

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_CAMERA_UTIL_H_