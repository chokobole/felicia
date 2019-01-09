#ifndef FELICIA_CORE_PLATFORM_PLATFORM_ERROR_H_
#define FELICIA_CORE_PLATFORM_PLATFORM_ERROR_H_

#include <string>

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/platform.h"
#include "felicia/core/lib/net/errors.h"

#if defined(PLATFORM_WINDOWS)
// This has already been defined in the header, but defining it again as DWORD
// ensures that the type used in the header is equivalent to DWORD. If not,
// the redefinition is a compile error.
typedef unsigned long PlatformErrorCode;
#elif defined(PLATFORM_POSIX)
typedef int PlatformErrorCode;
#else
#error Define the appropriate PLATFORM_<foo> macro for this platform
#endif

namespace felicia {

// Alias for ::Get/SetLastError() on Windows and errno on POSIX. Avoids having
// to pull in windows.h just for Get/SetLastError() and DWORD.
EXPORT PlatformErrorCode GetLastPlatformErrorCode();
EXPORT void SetLastPlatformErrorCode(PlatformErrorCode error_code);
EXPORT std::string PlatformErrorCodeToString(PlatformErrorCode error_code);

// Map system error code to Error.
EXPORT net::Error MapSystemError(PlatformErrorCode error_code);

}  // namespace felicia

#endif  // FELICIA_CORE_PLATFORM_PLATFORM_ERROR_H_