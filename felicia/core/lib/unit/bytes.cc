#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

// if |SaturateAdd| and |SaturateSub| are inside the bytes.h
// then gold linker complains that they have multiple definitions.
// static
Bytes Bytes::SaturateAdd(size_t value, size_t value2) {
  ::base::CheckedNumeric<size_t> rv(value);
  rv += value2;
  if (rv.IsValid()) return Bytes(rv.ValueOrDie());
  return Bytes::Max();
}

// static
Bytes Bytes::SaturateSub(size_t value, size_t value2) {
  ::base::CheckedNumeric<size_t> rv(value);
  rv -= value2;
  if (rv.IsValid()) return Bytes(rv.ValueOrDie());
  return Bytes();
}

std::ostream& operator<<(std::ostream& os, Bytes bytes) {
  os << ::base::NumberToString(bytes.bytes()) << " bytes";
  return os;
}

}  // namespace felicia