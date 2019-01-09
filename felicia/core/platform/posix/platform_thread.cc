// Copyright (c) 2012 The Chromium Authors. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// =====================================================================
// Modifications copyright (C) 2019 felicia

#include "felicia/core/platform/platform_thread.h"

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <memory>

#include "felicia/core/lib/base/logging.h"
#include "felicia/core/lib/base/no_destructor.h"

#include "felicia/core/lib/base/platform.h"
#if defined(PLATFORM_LINUX)
#include <sys/syscall.h>
#endif

namespace felicia {

void InitThreading() {}

void TerminateOnThread() {}

size_t GetDefaultThreadStackSize(const pthread_attr_t& attributes) { return 0; }

namespace {

struct ThreadParams {
  ThreadParams() : delegate(nullptr), joinable(false) {}

  PlatformThread::Delegate* delegate;
  bool joinable;
};

void* ThreadFunc(void* params) {
  PlatformThread::Delegate* delegate = nullptr;

  {
    std::unique_ptr<ThreadParams> thread_params(
        static_cast<ThreadParams*>(params));

    delegate = thread_params->delegate;
  }

  // ThreadIdNameManager::GetInstance()->RegisterThread(
  //     PlatformThread::CurrentHandle().platform_handle(),
  //     PlatformThread::CurrentId());

  delegate->ThreadMain();

  // ThreadIdNameManager::GetInstance()->RemoveName(
  //     PlatformThread::CurrentHandle().platform_handle(),
  //     PlatformThread::CurrentId());

  felicia::TerminateOnThread();
  return nullptr;
}

bool CreateThread(size_t stack_size, bool joinable,
                  PlatformThread::Delegate* delegate,
                  PlatformThreadHandle* thread_handle) {
  DCHECK(thread_handle);
  felicia::InitThreading();

  pthread_attr_t attributes;
  pthread_attr_init(&attributes);

  // Pthreads are joinable by default, so only specify the detached
  // attribute if the thread should be non-joinable.
  if (!joinable)
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);

  // Get a better default if available.
  if (stack_size == 0)
    stack_size = felicia::GetDefaultThreadStackSize(attributes);

  if (stack_size > 0) pthread_attr_setstacksize(&attributes, stack_size);

  std::unique_ptr<ThreadParams> params(new ThreadParams);
  params->delegate = delegate;
  params->joinable = joinable;

  pthread_t handle;
  int err = pthread_create(&handle, &attributes, ThreadFunc, params.get());
  bool success = !err;
  if (success) {
    // ThreadParams should be deleted on the created thread after used.
    params.release();
  } else {
    // Value of |handle| is undefined if pthread_create fails.
    handle = 0;
    errno = err;
    LOG(ERROR) << PlatformErrorCodeToString(errno);
  }
  *thread_handle = PlatformThreadHandle(handle);

  pthread_attr_destroy(&attributes);

  return success;
}

#if defined(PLATFORM_LINUX)

// Store the thread ids in local storage since calling the SWI can
// expensive and PlatformThread::CurrentId is used liberally. Clear
// the stored value after a fork() because forking changes the thread
// id. Forking without going through fork() (e.g. clone()) is not
// supported, but there is no known usage. Using thread_local is
// fine here (despite being banned) since it is going to be allowed
// but is blocked on a clang bug for Mac (https://crbug.com/829078)
// and we can't use ThreadLocalStorage because of re-entrancy due to
// CHECK/DCHECKs.
thread_local pid_t g_thread_id = -1;

void ClearTidCache() { g_thread_id = -1; }

class InitAtFork {
 public:
  InitAtFork() { pthread_atfork(nullptr, nullptr, ClearTidCache); }
};

#endif  // defined(PLATFORM_LINUX)

}  // namespace

// static
PlatformThreadId PlatformThread::CurrentId() {
  // Pthreads doesn't have the concept of a thread ID, so we have to reach down
  // into the kernel.
#if defined(PLATFORM_MACOSX)
  return pthread_mach_thread_np(pthread_self());
#elif defined(PLATFORM_LINUX)
  static NoDestructor<InitAtFork> init_at_fork;
  if (g_thread_id == -1) {
    g_thread_id = syscall(__NR_gettid);
  } else {
    DCHECK_EQ(g_thread_id, syscall(__NR_gettid))
        << "Thread id stored in TLS is different from thread id returned by "
           "the system. It is likely that the process was forked without going "
           "through fork().";
  }
  return g_thread_id;
#elif defined(PLATFORM_ANDROID)
  return gettid();
#endif
}

// static
PlatformThreadRef PlatformThread::CurrentRef() {
  return PlatformThreadRef(pthread_self());
}

// static
PlatformThreadHandle PlatformThread::CurrentHandle() {
  return PlatformThreadHandle(pthread_self());
}

// static
void PlatformThread::YieldCurrentThread() { sched_yield(); }

// static
void PlatformThread::Sleep(absl::Duration duration) {
  struct timespec sleep_time, remaining;

  // Break the duration into seconds and nanoseconds.
  // NOTE: TimeDelta's microseconds are int64s while timespec's
  // nanoseconds are longs, so this unpacking must prevent overflow.
  sleep_time.tv_sec = absl::ToInt64Seconds(duration);
  duration -= absl::Seconds(sleep_time.tv_sec);
  sleep_time.tv_nsec = absl::ToInt64Nanoseconds(duration);

  while (nanosleep(&sleep_time, &remaining) == -1 && errno == EINTR)
    sleep_time = remaining;
}

// static
// const char* PlatformThread::GetName() {
//   return ThreadIdNameManager::GetInstance()->GetName(CurrentId());
// }

// static
bool PlatformThread::Create(size_t stack_size, Delegate* delegate,
                            PlatformThreadHandle* thread_handle) {
  return CreateThread(stack_size, true /* joinable thread */, delegate,
                      thread_handle);
}

// static
bool PlatformThread::CreateNonJoinable(size_t stack_size, Delegate* delegate) {
  PlatformThreadHandle unused;

  return CreateThread(stack_size, false /* non-joinable thread */, delegate,
                      &unused);
}

// static
void PlatformThread::Join(PlatformThreadHandle thread_handle) {
  CHECK_EQ(0, pthread_join(thread_handle.platform_handle(), nullptr));
}

// static
void PlatformThread::Detach(PlatformThreadHandle thread_handle) {
  CHECK_EQ(0, pthread_detach(thread_handle.platform_handle()));
}

// static
size_t PlatformThread::GetDefaultThreadStackSize() {
  pthread_attr_t attributes;
  pthread_attr_init(&attributes);
  size_t default_thread_stack_size =
      felicia::GetDefaultThreadStackSize(attributes);
  pthread_attr_destroy(&attributes);
  return default_thread_stack_size;
}

}  // namespace felicia
