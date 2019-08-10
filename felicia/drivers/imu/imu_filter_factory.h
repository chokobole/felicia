#ifndef FELICIA_DRIVERS_IMU_IMU_FILTER_FACTORY_H_
#define FELICIA_DRIVERS_IMU_IMU_FILTER_FACTORY_H_

#include <memory>

#include "felicia/drivers/imu/imu_filter_interface.h"

namespace felicia {
namespace drivers {

class ImuFilterFactory {
 public:
  enum ImuFilterKind {
    COMPLEMENTARY_FILTER_KIND,
    MADGWICK_FILTER_KIND,
  };

  static std::unique_ptr<ImuFilterInterface> NewImuFilter(ImuFilterKind kind);
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_FILTER_FACTORY_H_