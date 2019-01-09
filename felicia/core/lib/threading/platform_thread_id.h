#ifndef FELICIA_CORE_LIB_THREADING_PLATFORM_THREAD_ID_H_
#define FELICIA_CORE_LIB_THREADING_PLATFORM_THREAD_ID_H_

#include "felicia/core/lib/base/platform.h"
#if defined(PLATFORM_MACOSX)
#include <mach/mach_types.h>
#elif defined(PLATFORM_POSIX)
#include <unistd.h>
#elif defined(PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace felicia {

#if defined(PLATFORM_MACOSX)
typedef mach_port_t PlatformThreadId;
#elif defined(PLATFORM_POSIX)
typedef pid_t PlatformThreadId;
#elif defined(PLATFORM_WINDOWS)
typedef DWORD PlatformThreadId;
#endif

const PlatformThreadId kInvalidThreadId = 0;

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_THREADING_PLATFORM_THREAD_ID_H_