#include "felicia/core/lib/file/file_util.h"

#if defined(OS_POSIX)
#include <fcntl.h>

#include "third_party/chromium/base/posix/eintr_wrapper.h"
#include "third_party/chromium/base/strings/utf_string_conversions.h"
#elif defined(OS_WIN)
#include "third_party/chromium/base/strings/utf_string_conversions.h"
#endif

namespace felicia {

#if defined(OS_POSIX)
bool SetBlocking(int fd, int blocking) {
  int flags = fcntl(fd, F_GETFL);
  if (flags == -1) return false;

  if (blocking) {
    if (!(flags & O_NONBLOCK)) return true;
    flags &= ~O_NONBLOCK;
  } else {
    if (flags & O_NONBLOCK) return true;
    flags |= O_NONBLOCK;
  }

  if (HANDLE_EINTR(fcntl(fd, F_SETFL, flags)) == -1) return false;
  return true;
}
#endif

base::FilePath::StringType ToFilePathString(const std::string& file_path) {
#if defined(OS_WIN)
  return base::UTF8ToUTF16(file_path);
#else
  return file_path;
#endif
}

}  // namespace felicia
