#include "felicia/core/message/header.h"

namespace felicia {

// static
bool Header::FromBytes(const char* bytes, Header* header) {
  uint32_t key = *reinterpret_cast<const uint32_t*>(bytes);
  if (key != kMessageMagicValue) return false;
  bytes += sizeof(uint32_t);
  header->set_size(*reinterpret_cast<const uint32_t*>(bytes));

  return true;
}

}  // namespace felicia