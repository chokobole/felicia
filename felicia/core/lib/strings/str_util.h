#ifndef FELICIA_CORE_LIB_STRINGS_STR_UTIL_H_
#define FELICIA_CORE_LIB_STRINGS_STR_UTIL_H_

#include "third_party/chromium/base/strings/string_piece.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

// If |s| starts with |expected|, consume it and return true.
// Otherwise, return false.
FEL_EXPORT bool ConsumePrefix(base::StringPiece* s, base::StringPiece expected);

// If |s| ends with |expected|, remove it and return true.
// Otherwise, return false.
FEL_EXPORT bool ConsumeSuffix(base::StringPiece* s, base::StringPiece expected);

// If |s| starts with |expected|, return true. Otherwise, return false.
FEL_EXPORT bool StartsWith(base::StringPiece s, base::StringPiece expected);

// If |s| ends with |expected|, return true. Otherwise, return false.
FEL_EXPORT bool EndsWith(base::StringPiece s, base::StringPiece expected);

// If |s| contains |expected|, return true. Otherwise, return false.
FEL_EXPORT bool Contains(base::StringPiece s, base::StringPiece expected);

FEL_EXPORT std::string BoolToString(bool b);

}  // namespace felicia

#endif  // BASE_STRINGS_STR_UTIL_H_
