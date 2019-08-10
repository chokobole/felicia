#include "felicia/drivers/lidar/lidar_interface.h"

namespace felicia {
namespace drivers {

LidarInterface::LidarInterface(const LidarEndpoint& lidar_endpoint)
    : lidar_endpoint_(lidar_endpoint) {}

LidarInterface::~LidarInterface() = default;

bool LidarInterface::IsInitialized() const {
  return lidar_state_.IsInitialized();
}

bool LidarInterface::IsStarted() const { return lidar_state_.IsStarted(); }

bool LidarInterface::IsStopped() const { return lidar_state_.IsStopped(); }

}  // namespace drivers
}  // namespace felicia