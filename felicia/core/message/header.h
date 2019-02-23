#ifndef FELICIA_CORE_MESSAGE_HEADER_H_
#define FELICIA_CORE_MESSAGE_HEADER_H_

#include <stdint.h>

#include <type_traits>

#include "felicia/core/lib/base/export.h"

namespace felicia {

class EXPORT Header {
 public:
  Header() = default;

  static Header FromBytes(const char* bytes);

  uint32_t size() const { return size_; }
  void set_size(uint32_t size) { size_ = size; }

 private:
  uint32_t size_;
};

static_assert(std::is_trivially_copyable<Header>::value, "");

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_HEADER_H_