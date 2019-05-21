#ifndef FELICIA_DRIVERS_IMU_IMU_FILTER_FACTORY_H_
#define FELICIA_DRIVERS_IMU_IMU_FILTER_FACTORY_H_

#include <memory>

#include "felicia/drivers/imu/imu_filter_interface.h"

namespace felicia {

class ImuFilterFactory {
 public:
  enum ImuFilterKind {
    ComplementaryFilterKind,
  };

  static std::unique_ptr<ImuFilterInterface> NewImuFilter(ImuFilterKind kind);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_FILTER_FACTORY_H_