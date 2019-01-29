#include "felicia/core/lib/base/location.h"

#include "absl/hash/hash_testing.h"
#include "gtest/gtest.h"

namespace felicia {
namespace {
#if defined(__clang__)
TEST(Location, Hash) {
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
      Location(),
      FROM_HERE,
  }));
}
#endif

}  // namespace
}  // namespace felicia
