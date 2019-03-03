#ifndef FELICIA_CORE_MESSAGE_HEADER_H_
#define FELICIA_CORE_MESSAGE_HEADER_H_

#include <stdint.h>

#include <type_traits>

#include "felicia/core/lib/base/export.h"

namespace felicia {

class EXPORT Header {
 public:
  constexpr Header() : magic_value_(kMessageMagicValue) {}

  static bool FromBytes(const char* bytes, Header* header);

  uint32_t size() const { return size_; }
  void set_size(uint32_t size) { size_ = size; }

 private:
  uint32_t magic_value_ = 0;
  uint32_t size_ = 0;

  static constexpr uint32_t kMessageMagicValue = 0x12452903u;
};

static_assert(std::is_trivially_copyable<Header>::value, "");

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_HEADER_H_