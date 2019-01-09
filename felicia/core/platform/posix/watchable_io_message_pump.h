// Copyright 2018 The Chromium Authors. All rights reserved.
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

#ifndef FELICIA_CORE_PLATFORM_POSIX_WATCHABLE_IO_MESSAGE_PUMP_H_
#define FELICIA_CORE_PLATFORM_POSIX_WATCHABLE_IO_MESSAGE_PUMP_H_

#include "felicia/core/lib/base/location.h"
#include "felicia/core/lib/base/macros.h"

namespace felicia {

class WatchableIOMessagePump {
 public:
  // Used with WatchFileDescriptor to asynchronously monitor the I/O readiness
  // of a file descriptor.
  class FdWatcher {
   public:
    virtual void OnFileCanReadWithoutBlocking(int fd) = 0;
    virtual void OnFileCanWriteWithoutBlocking(int fd) = 0;

   protected:
    virtual ~FdWatcher() = default;
  };

  class FdWatchControllerInterface {
   public:
    explicit FdWatchControllerInterface(const Location& from_here);
    // Subclasses must call StopWatchingFileDescriptor() in their destructor
    // (this parent class cannot generically do it for them as it must usually
    // be invoked before they destroy their state which happens before the
    // parent destructor is invoked).
    virtual ~FdWatchControllerInterface();

    // NOTE: This method isn't called StopWatching() to avoid confusion with the
    // win32 ObjectWatcher class. While this doesn't really need to be virtual
    // as there's only one impl per platform and users don't use pointers to the
    // base class. Having this interface forces implementers to share similar
    // implementations (a problem in the past).

    // Stop watching the FD, always safe to call.  No-op if there's nothing to
    // do.
    virtual bool StopWatchingFileDescriptor() = 0;

    const Location& created_from_location() const {
      return created_from_location_;
    }

   private:
    const Location created_from_location_;

    DISALLOW_COPY_AND_ASSIGN(FdWatchControllerInterface);
  };

  enum Mode {
    WATCH_READ = 1 << 0,
    WATCH_WRITE = 1 << 1,
    WATCH_READ_WRITE = WATCH_READ | WATCH_WRITE
  };

  // Every subclass of WatchableIOMessagePump must provide a
  // WatchFileDescriptor() which has the following signature where
  // |FdWatchController| must be the complete type based on
  // FdWatchControllerInterface.

  // Registers |delegate| with the current thread's message loop so that its
  // methods are invoked when file descriptor |fd| becomes ready for reading or
  // writing (or both) without blocking.  |mode| selects ready for reading, for
  // writing, or both.  See "enum Mode" above.  |controller| manages the
  // lifetime of registrations. ("Registrations" are also ambiguously called
  // "events" in many places, for instance in libevent.)  It is an error to use
  // the same |controller| for different file descriptors; however, the same
  // controller can be reused to add registrations with a different |mode|.  If
  // |controller| is already attached to one or more registrations, the new
  // registration is added onto those.  If an error occurs while calling this
  // method, any registration previously attached to |controller| is removed.
  // Returns true on success.  Must be called on the same thread the MessagePump
  // is running on.
  // bool WatchFileDescriptor(int fd,
  //                          bool persistent,
  //                          int mode,
  //                          FdWatchController* controller,
  //                          FdWatcher* delegate) = 0;
};

}  // namespace felicia

#endif  // FELICIA_CORE_PLATFORM_POSIX_WATCHABLE_IO_MESSAGE_PUMP_H_