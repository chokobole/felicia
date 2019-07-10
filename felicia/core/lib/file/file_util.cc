#include "felicia/core/lib/file/file_util.h"

#if defined(OS_POSIX)
#include <fcntl.h>

#include "third_party/chromium/base/posix/eintr_wrapper.h"

namespace felicia {

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

}  // namespace felicia
#endif