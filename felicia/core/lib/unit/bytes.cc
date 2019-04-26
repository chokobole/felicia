#include "felicia/core/lib/unit/bytes.h"

namespace felicia {

std::ostream& operator<<(std::ostream& os, Bytes bytes) {
  os << ::base::NumberToString(bytes.bytes());
  return os;
}

}  // namespace felicia