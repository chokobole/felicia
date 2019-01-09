#include "felicia/core/platform/platform_error.h"

#include <windows.h>

#include "absl/strings/str_format.h"

#include "felicia/core/lib/base/stl_util.h"
#include "felicia/core/lib/strings/string_util.h"

#if defined(PLATFORM_WINDOWS)
// This has already been defined in the header, but defining it again as DWORD
// ensures that the type used in the header is equivalent to DWORD. If not,
// the redefinition is a compile error.
typedef DWORD SystemErrorCode;
#endif

namespace felicia {

PlatformErrorCode GetLastPlatformErrorCode() { return ::GetLastError(); }

void SetLastPlatformErrorCode(PlatformErrorCode error_code) {
  ::SetLastError(error_code);
}

std::string PlatformErrorCodeToString(PlatformErrorCode error_code) {
  const int kErrorMessageBufferSize = 256;
  char msgbuf[kErrorMessageBufferSize];
  DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
  DWORD len = ::FormatMessageA(flags, nullptr, error_code, 0, msgbuf,
                               size(msgbuf), nullptr);
  if (len) {
    // Messages returned by system end with line breaks.
    return felicia::CollapseWhitespaceASCII(msgbuf, true) +
           absl::StrFormat(" (0x%lX)", error_code);
  }
  return absl::StrFormat("Error (0x%lX) while retrieving error. (0x%lX)",
                         GetLastError(), error_code);
}

}  // namespace felicia