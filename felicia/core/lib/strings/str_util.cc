#include "felicia/core/lib/strings/str_util.h"

#include <algorithm>

#include "third_party/chromium/base/logging.h"
#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/utf_string_conversions.h"

#include "felicia/core/lib/strings/str_util.h"

namespace felicia {
namespace strings {

bool ConsumePrefix(base::StringPiece* s, base::StringPiece expected) {
  if (StartsWith(*s, expected)) {
    s->remove_prefix(expected.size());
    return true;
  }
  return false;
}

bool ConsumeSuffix(base::StringPiece* s, base::StringPiece expected) {
  if (EndsWith(*s, expected)) {
    s->remove_suffix(expected.size());
    return true;
  }
  return false;
}

bool StartsWith(base::StringPiece s, base::StringPiece expected) {
  return base::StartsWith(s, expected, base::CompareCase::SENSITIVE);
}

bool EndsWith(base::StringPiece s, base::StringPiece expected) {
  return base::EndsWith(s, expected, base::CompareCase::SENSITIVE);
}

bool Equals(base::StringPiece s, base::StringPiece expected) {
  return base::EqualsASCII(ASCIIToUTF16(s), expected);
}

bool Contains(base::StringPiece s, base::StringPiece expected) {
  return s.find(expected) != base::StringPiece::npos;
}

StringComparator::StringComparator(base::StringPiece text) : text_(text) {}

bool StringComparator::operator()(const base::StringPiece text) {
  return Equals(text_, text);
}

std::string BoolToString(bool b) { return b ? "true" : "false"; }

}  // namespace strings
}  // namespace felicia
