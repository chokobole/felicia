#include "felicia/drivers/imu/imu_filter_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"

#include "felicia/drivers/imu/complementary_filter/complementary_filter.h"

namespace felicia {

std::unique_ptr<ImuFilterInterface> ImuFilterFactory::NewImuFilter(
    ImuFilterKind kind) {
  if (kind == ComplementaryFilterKind) {
    return ::base::WrapUnique(new ComplementaryFilter());
  }

  return nullptr;
}

}  // namespace felicia