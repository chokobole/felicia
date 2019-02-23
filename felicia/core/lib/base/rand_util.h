#ifndef FELICIA_CORE_LIB_BASE_RAND_UTIL_H_
#define FELICIA_CORE_LIB_BASE_RAND_UTIL_H_

#include <stddef.h>
#include <string>

#include "felicia/core/lib/base/export.h"

namespace felicia {

EXPORT std::string RandAlphaDigit(size_t length);

}

#endif  // FELICIA_CORE_LIB_BASE_RAND_UTIL_H_