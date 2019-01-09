#ifndef FELICIA_CORE_LIB_THREADING_PLATFORM_THREAD_HANDLE_H_
#define FELICIA_CORE_LIB_THREADING_PLATFORM_THREAD_HANDLE_H_

#include "felicia/core/lib/base/export.h"

#include "felicia/core/lib/base/platform.h"
#if defined(PLATFORM_POSIX)
#include <pthread.h>
#endif

namespace felicia {

#if defined(PLATFORM_POSIX)
typedef pthread_t Handle;
#elif defined(PLATFORM_WINDOWS)
typedef void* Handle;
#endif

// Used to operate on threads.
class EXPORT PlatformThreadHandle {
 public:
  constexpr PlatformThreadHandle() : handle_(0) {}

  explicit constexpr PlatformThreadHandle(Handle handle) : handle_(handle) {}

  bool is_equal(const PlatformThreadHandle& other) const {
    return handle_ == other.handle_;
  }

  bool is_null() const { return !handle_; }

  Handle platform_handle() const { return handle_; }

 private:
  Handle handle_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_THREADING_PLATFORM_THREAD_HANDLE_H_