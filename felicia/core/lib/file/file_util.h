#ifndef FELICIA_CORE_LIB_FILE_FILE_UTIL_H_
#define FELICIA_CORE_LIB_FILE_FILE_UTIL_H_

#include "third_party/chromium/build/build_config.h"
#include "third_party/chromium/base/files/file_path.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

#if defined(OS_POSIX)
EXPORT bool SetBlocking(int fd, int blocking);
#endif

EXPORT ::base::FilePath::StringType ToFilePathString(
    const std::string& file_path);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_FILE_FILE_UTIL_H_