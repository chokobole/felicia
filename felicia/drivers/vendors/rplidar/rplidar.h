#ifndef FELICIA_DRIVERS_VENDORS_RPLIDAR_H_
#define FELICIA_DRIVERS_VENDORS_RPLIDAR_H_

#include <rplidar.h>

#include "felicia/drivers/lidar/lidar_interface.h"

namespace felicia {

class RPlidar : public LidarInterface {
 public:
  ~RPlidar();

  Status Init() override;
  Status Start() override;
  Status Stop() override;

 private:
  friend class RPlidarFactory;

  RPlidar(const LidarEndpoint& lidar_endpoint);

  std::unique_ptr<rp::standalone::rplidar::RPlidarDriver> driver_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_VENDORS_RPLIDAR_H_