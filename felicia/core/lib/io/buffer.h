#ifndef FELICIA_CORE_LIB_IO_BUFFER_H_
#define FELICIA_CORE_LIB_IO_BUFFER_H_

#include <stddef.h>

#include "felicia/core/lib/base/export.h"

namespace felicia {

class EXPORT Buffer {
 public:
  Buffer(char* data, size_t size);
  explicit Buffer(size_t size);
  ~Buffer();

  char* data() const { return data_; }
  size_t size() const { return size_; }

 private:
  char* data_;
  size_t size_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_IO_BUFFER_H_