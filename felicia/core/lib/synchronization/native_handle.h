#ifndef FELICIA_CORE_LIB_SYNCHRONIZATION_NATIVE_HANDLE_H_
#define FELICIA_CORE_LIB_SYNCHRONIZATION_NATIVE_HANDLE_H_

#include "felicia/core/lib/base/platform.h"
#if defined(PLATFORM_POSIX)
#include <pthread.h>
#elif defined(PLATFORM_WINDOWS)
#include <windows.h>
#error Define the appropriate PLATFORM_<foo> macro for this platform
#endif

namespace felicia {

#if defined(PLATFORM_POSIX)
using NativeHandle = pthread_mutex_t;
#elif defined(PLATFORM_WINDOWS)
using NativeHandle = SRWLOCK;
#endif

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_SYNCHRONIZATION_NATIVE_HANDLE_H_