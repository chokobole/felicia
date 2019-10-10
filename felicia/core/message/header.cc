#include "felicia/core/message/header.h"

namespace felicia {

Header::Header() : magic_value_(kMessageMagicValue) {}

// static
bool Header::FromBytes(const char* bytes, Header* header,
                       bool use_ros_channel) {
  header->set_size(*reinterpret_cast<const uint32_t*>(bytes));
  if (!use_ros_channel) {
    bytes += sizeof(uint32_t);
    uint32_t key = *reinterpret_cast<const uint32_t*>(bytes);
    if (key != kMessageMagicValue) return false;
  }

  return true;
}

uint32_t Header::size() const { return size_; }

void Header::set_size(uint32_t size) { size_ = size; }

}  // namespace felicia