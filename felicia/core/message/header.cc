#include "felicia/core/message/header.h"

namespace felicia {

// static
Header Header::FromBytes(const char* bytes) {
  Header header;
  header.set_size(*reinterpret_cast<const uint32_t*>(bytes));

  return header;
}

}  // namespace felicia