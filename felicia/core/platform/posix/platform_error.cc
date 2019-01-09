#include "felicia/core/platform/platform_error.h"

#include <errno.h>

#include "absl/strings/str_format.h"

#include "felicia/core/lib/base/logging.h"

namespace felicia {

PlatformErrorCode GetLastPlatformErrorCode() { return errno; }

void SetLastPlatformErrorCode(PlatformErrorCode error_code) {
  errno = error_code;
}

std::string PlatformErrorCodeToString(PlatformErrorCode error_code) {
  return absl::StrFormat("%s (%d)", strerror(error_code), error_code);
}

net::Error MapSystemError(PlatformErrorCode error_code) {
  if (error_code != 0) {
    // DVLOG(2) << "Error " << error_code;
    DLOG(WARNING) << "Error " << error_code;
  }

  // There are numerous posix error codes, but these are the ones we thus far
  // find interesting.
  switch (error_code) {
    case EAGAIN:
#if EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:
#endif
      return net::ERR_IO_PENDING;
    case EACCES:
      return net::ERR_ACCESS_DENIED;
    case ENETDOWN:
      return net::ERR_INTERNET_DISCONNECTED;
    case ETIMEDOUT:
      return net::ERR_TIMED_OUT;
    case ECONNRESET:
    case ENETRESET:  // Related to keep-alive.
    case EPIPE:
      return net::ERR_CONNECTION_RESET;
    case ECONNABORTED:
      return net::ERR_CONNECTION_ABORTED;
    case ECONNREFUSED:
      return net::ERR_CONNECTION_REFUSED;
    case EHOSTUNREACH:
    case EHOSTDOWN:
    case ENETUNREACH:
    case EAFNOSUPPORT:
      return net::ERR_ADDRESS_UNREACHABLE;
    case EADDRNOTAVAIL:
      return net::ERR_ADDRESS_INVALID;
    case EMSGSIZE:
      return net::ERR_MSG_TOO_BIG;
    case ENOTCONN:
      return net::ERR_SOCKET_NOT_CONNECTED;
    case EISCONN:
      return net::ERR_SOCKET_IS_CONNECTED;
    case EINVAL:
      return net::ERR_INVALID_ARGUMENT;
    case EADDRINUSE:
      return net::ERR_ADDRESS_IN_USE;
    case E2BIG:  // Argument list too long.
      return net::ERR_INVALID_ARGUMENT;
    case EBADF:  // Bad file descriptor.
      return net::ERR_INVALID_HANDLE;
    case EBUSY:  // Device or resource busy.
      return net::ERR_INSUFFICIENT_RESOURCES;
    case ECANCELED:  // Operation canceled.
      return net::ERR_ABORTED;
    case EDEADLK:  // Resource deadlock avoided.
      return net::ERR_INSUFFICIENT_RESOURCES;
    case EDQUOT:  // Disk quota exceeded.
      return net::ERR_FILE_NO_SPACE;
    case EEXIST:  // File exists.
      return net::ERR_FILE_EXISTS;
    case EFAULT:  // Bad address.
      return net::ERR_INVALID_ARGUMENT;
    case EFBIG:  // File too large.
      return net::ERR_FILE_TOO_BIG;
    case EISDIR:  // Operation not allowed for a directory.
      return net::ERR_ACCESS_DENIED;
    case ENAMETOOLONG:  // Filename too long.
      return net::ERR_FILE_PATH_TOO_LONG;
    case ENFILE:  // Too many open files in system.
      return net::ERR_INSUFFICIENT_RESOURCES;
    case ENOBUFS:  // No buffer space available.
      return net::ERR_NO_BUFFER_SPACE;
    case ENODEV:  // No such device.
      return net::ERR_INVALID_ARGUMENT;
    case ENOENT:  // No such file or directory.
      return net::ERR_FILE_NOT_FOUND;
    case ENOLCK:  // No locks available.
      return net::ERR_INSUFFICIENT_RESOURCES;
    case ENOMEM:  // Not enough space.
      return net::ERR_OUT_OF_MEMORY;
    case ENOSPC:  // No space left on device.
      return net::ERR_FILE_NO_SPACE;
    case ENOSYS:  // Function not implemented.
      return net::ERR_NOT_IMPLEMENTED;
    case ENOTDIR:  // Not a directory.
      return net::ERR_FILE_NOT_FOUND;
    case ENOTSUP:  // Operation not supported.
      return net::ERR_NOT_IMPLEMENTED;
    case EPERM:  // Operation not permitted.
      return net::ERR_ACCESS_DENIED;
    case EROFS:  // Read-only file system.
      return net::ERR_ACCESS_DENIED;
    case ETXTBSY:  // Text file busy.
      return net::ERR_ACCESS_DENIED;
    case EUSERS:  // Too many users.
      return net::ERR_INSUFFICIENT_RESOURCES;
    case EMFILE:  // Too many open files.
      return net::ERR_INSUFFICIENT_RESOURCES;

    case 0:
      return net::OK;
    default:
      LOG(WARNING) << "Unknown error " << strerror(error_code) << " ("
                   << error_code << ") mapped to net::ERR_FAILED";
      return net::ERR_FAILED;
  }
}

}  // namespace felicia