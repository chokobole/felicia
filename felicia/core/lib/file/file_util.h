#ifndef FELICIA_CORE_LIB_FILE_FILE_UTIL_H_
#define FELICIA_CORE_LIB_FILE_FILE_UTIL_H_

#include <memory>

#include "third_party/chromium/base/files/file_path.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {

#if defined(OS_POSIX)
EXPORT bool SetBlocking(int fd, int blocking);
#endif

EXPORT base::FilePath ToFilePath(const std::string& file_path);

EXPORT bool ReadFile(const base::FilePath& path, std::unique_ptr<char[]>* out,
                     size_t* out_len);

EXPORT bool WriteFile(const base::FilePath& path, const char* buf,
                      size_t buf_len);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_FILE_FILE_UTIL_H_