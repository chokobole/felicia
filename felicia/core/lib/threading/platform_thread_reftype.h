#ifndef FELICIA_CORE_LIB_THREADING_PLATFORM_THREAD_REFTYPE_H_
#define FELICIA_CORE_LIB_THREADING_PLATFORM_THREAD_REFTYPE_H_

#include "felicia/core/lib/base/export.h"

#include "felicia/core/lib/base/platform.h"
#if defined(PLATFORM_POSIX)
#include <pthread.h>
#elif defined(PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace felicia {

#if defined(PLATFORM_POSIX)
typedef pthread_t RefType;
#elif defined(PLATFORM_WINDOWS)
typedef DWORD RefType;
#endif

// Used for thread checking and debugging.
// Meant to be as fast as possible.
// These are produced by PlatformThread::CurrentRef(), and used to later
// check if we are on the same thread or not by using ==. These are safe
// to copy between threads, but can't be copied to another process as they
// have no meaning there. Also, the internal identifier can be re-used
// after a thread dies, so a PlatformThreadRef cannot be reliably used
// to distinguish a new thread from an old, dead thread.
class EXPORT PlatformThreadRef {
 public:
  constexpr PlatformThreadRef() : id_(0) {}

  explicit constexpr PlatformThreadRef(RefType id) : id_(id) {}

  bool operator==(PlatformThreadRef other) const { return id_ == other.id_; }

  bool operator!=(PlatformThreadRef other) const { return id_ != other.id_; }

  bool is_null() const { return id_ == 0; }

 private:
  RefType id_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_THREADING_PLATFORM_THREAD_REFTYPE_H_