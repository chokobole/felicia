#include "felicia/drivers/imu/imu_filter_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"

#include "felicia/drivers/imu/complementary_filter/complementary_filter.h"
#include "felicia/drivers/imu/madgwick_filter/madgwick_filter.h"

namespace felicia {

std::unique_ptr<ImuFilterInterface> ImuFilterFactory::NewImuFilter(
    ImuFilterKind kind) {
  if (kind == COMPLEMENTARY_FILTER_KIND) {
    return ::base::WrapUnique(new ComplementaryFilter());
  } else if (kind == MADGWICK_FILTER_KIND) {
    return ::base::WrapUnique(new MadgwickFilter());
  }

  return nullptr;
}

}  // namespace felicia