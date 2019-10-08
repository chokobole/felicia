#ifndef FELICIA_CORE_MESSAGE_HEADER_H_
#define FELICIA_CORE_MESSAGE_HEADER_H_

#include <stdint.h>

#include <type_traits>

#include "felicia/core/lib/base/export.h"

namespace felicia {

class EXPORT Header {
 public:
  Header();

  static bool FromBytes(const char* bytes, Header* header,
                        bool receive_from_ros);

  uint32_t size() const;
  void set_size(uint32_t size);

 private:
  uint32_t size_ = 0;
  uint32_t magic_value_ = 0;

  static constexpr uint32_t kMessageMagicValue = 0x12452903u;
};

static_assert(std::is_trivially_copyable<Header>::value, "");

}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_HEADER_H_