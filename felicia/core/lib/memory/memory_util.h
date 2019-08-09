#ifndef FELICIA_CORE_LIB_MEMORY_MEMORY_UTIL_H_
#define FELICIA_CORE_LIB_MEMORY_MEMORY_UTIL_H_

#include <memory>
#include <string>

#include "felicia/core/lib/base/export.h"

namespace felicia {

EXPORT bool IsSSOApplied(const std::string& str);

// Move ownership |str|'s data to returning unique_ptr |ret|.
template <typename T>
std::unique_ptr<T> StdStringToUniquePtr(const std::string& str) {
  if (IsSSOApplied(str)) {
    size_t length = str.length() / sizeof(T);
    std::unique_ptr<T> ret(new T[length]);
    memcpy(ret.get(), str.c_str(), str.length());
    return ret;
  } else {
    std::unique_ptr<T> ret;
    ret.reset(reinterpret_cast<T*>(const_cast<char*>(str.c_str())));
    return ret;
  }
}

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_MEMORY_MEMORY_UTIL_H_