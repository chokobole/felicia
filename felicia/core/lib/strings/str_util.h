#ifndef FELICIA_CORE_LIB_STRINGS_STR_UTIL_H_
#define FELICIA_CORE_LIB_STRINGS_STR_UTIL_H_

#include <functional>
#include <string>
#include <vector>

#include "third_party/chromium/base/strings/string_piece.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {
namespace strings {

// If |s| starts with |expected|, consume it and return true.
// Otherwise, return false.
EXPORT bool ConsumePrefix(::base::StringPiece* s, ::base::StringPiece expected);

// If |s| ends with |expected|, remove it and return true.
// Otherwise, return false.
EXPORT bool ConsumeSuffix(::base::StringPiece* s, ::base::StringPiece expected);

// If |s| equayl to |expected, return true. Otherwise, return false.
EXPORT bool Equals(::base::StringPiece s, ::base::StringPiece expected);

class StringComparator {
 public:
  explicit StringComparator(::base::StringPiece text);

  bool operator()(const ::base::StringPiece text);

 private:
  ::base::StringPiece text_;
};

}  // namespace strings
}  // namespace felicia

#endif  // BASE_STRINGS_STR_UTIL_H_
