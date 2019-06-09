#ifndef FELICIA_DRIVERS_LIDAR_LIDAR_INTERFACE_H_
#define FELICIA_DRIVERS_LIDAR_LIDAR_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/drivers/lidar/lidar_endpoint.h"
#include "felicia/drivers/lidar/lidar_state.h"

namespace felicia {

class EXPORT LidarInterface {
 public:
  LidarInterface(const LidarEndpoint& lidar_endpoint);
  virtual ~LidarInterface();

  virtual Status Init() = 0;
  virtual Status Start() = 0;
  virtual Status Stop() = 0;

  bool IsInitialized() const;
  bool IsStarted() const;
  bool IsStopped() const;

 protected:
  LidarEndpoint lidar_endpoint_;
  LidarState lidar_state_;

  StatusCallback status_callback_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_LIDAR_LIDAR_INTERFACE_H_