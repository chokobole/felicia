#include "felicia/core/lib/memory/memory_util.h"

#include "third_party/chromium/build/build_config.h"

namespace felicia {

#if defined(__clang__)
constexpr const int kSSOSize = 22;
#else
constexpr const int kSSOSize = 15;
#endif

bool IsSSOApplied(const std::string& str) { return str.length() <= kSSOSize; }

}  // namespace felicia