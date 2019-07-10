#ifndef FELICIA_CORE_LIB_FILE_FILE_UTIL_H_
#define FELICIA_CORE_LIB_FILE_FILE_UTIL_H_

#include "third_party/chromium/build/build_config.h"

namespace felicia {

#if defined(OS_POSIX)
bool SetBlocking(int fd, int blocking);
#endif

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_FILE_FILE_UTIL_H_