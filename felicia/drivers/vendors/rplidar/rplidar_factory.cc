#include "felicia/drivers/vendors/rplidar/rplidar_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"

namespace felicia {
namespace drivers {

// static
std::unique_ptr<RPlidar> RPlidarFactory::NewLidar(
    const LidarEndpoint& lidar_endpoint) {
  return base::WrapUnique(new RPlidar(lidar_endpoint));
}

}  // namespace drivers
}  // namespace felicia