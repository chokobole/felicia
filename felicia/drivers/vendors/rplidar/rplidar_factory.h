#ifndef FELICIA_DRIVERS_VENDORS_RPLIDAR_RPLIDAR_FACTORY_H_
#define FELICIA_DRIVERS_VENDORS_RPLIDAR_RPLIDAR_FACTORY_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/drivers/vendors/rplidar/rplidar.h"

namespace felicia {

class RPlidarFactory {
 public:
  static std::unique_ptr<RPlidar> NewLidar(const LidarEndpoint& lidar_endpoint);

  DISALLOW_COPY_AND_ASSIGN(RPlidarFactory);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_RPLIDAR_RPLIDAR_FACTORY_H_