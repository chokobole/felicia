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

#ifndef FELICIA_CORE_PLATFORM_POSIX_MESSAGE_PUMP_H_
#define FELICIA_CORE_PLATFORM_POSIX_MESSAGE_PUMP_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/macros.h"
#include "felicia/core/platform/posix/watchable_io_message_pump.h"

#include "absl/time/clock.h"
#include "libevent/include/event.h"

namespace felicia {

class EXPORT MessagePump : public MessagePumpBase,
                           public WatchableIOMessagePump {
 public:
  class FdWatchController : public FdWatchControllerInterface {
   public:
    explicit FdWatchController(const Location& from_here);

    // Implicitly calls StopWatchingFileDescriptor.
    ~FdWatchController() override;

    // FdWatchControllerInterface:
    bool StopWatchingFileDescriptor() override;

   private:
    friend class MessagePump;
    friend class MessagePumpTest;

    // Called by MessagePump.
    void Init(std::unique_ptr<event> e);

    // Used by MessagePump to take ownership of |event_|.
    std::unique_ptr<event> ReleaseEvent();

    void set_pump(MessagePump* pump) { pump_ = pump; }
    MessagePump* pump() const { return pump_; }

    void set_watcher(FdWatcher* watcher) { watcher_ = watcher; }

    void OnFileCanReadWithoutBlocking(int fd, MessagePump* pump);
    void OnFileCanWriteWithoutBlocking(int fd, MessagePump* pump);

    std::unique_ptr<event> event_;
    MessagePump* pump_ = nullptr;
    FdWatcher* watcher_ = nullptr;
    // If this pointer is non-NULL, the pointee is set to true in the
    // destructor.
    bool* was_destroyed_ = nullptr;

    DISALLOW_COPY_AND_ASSIGN(FdWatchController);
  };

  MessagePump();
  ~MessagePump() override;

  bool WatchFileDescriptor(int fd, bool persistent, int mode,
                           FdWatchController* controller, FdWatcher* delegate);

  // MessagePump methods:
  void Run(Delegate* delegate) override;
  void Quit() override;
  void ScheduleWork() override;
  void ScheduleDelayedWork(const absl::Time delayed_work_time) override;

 private:
  friend class MessagePumpTest;

  // Risky part of constructor.  Returns true on success.
  bool Init();

  // Called by libevent to tell us a registered FD can be read/written to.
  static void OnNotification(int fd, short flags, void* context);

  // Unix pipe used to implement ScheduleWork()
  // ... callback; called by libevent inside Run() when pipe is ready to read
  static void OnWakeup(int socket, short flags, void* context);

  // This flag is set to false when Run should return.
  bool keep_running_;

  // This flag is set when inside Run.
  bool in_run_;

  // This flag is set if libevent has processed I/O events.
  bool processed_io_events_;

  // The time at which we should call DoDelayedWork.
  absl::Time delayed_work_time_;

  // Libevent dispatcher. Watches all sockets registered with it, and sends
  // readiness callbacks when a socket is ready for I/O.
  event_base* event_base_;

  // ... write end; ScheduleWork() writes a single byte to it
  int wakeup_pipe_in_;
  // ... read end; OnWakeup reads it and then breaks Run() out of its sleep
  int wakeup_pipe_out_;
  // ... libevent wrapper for read end
  event* wakeup_event_;

  DISALLOW_COPY_AND_ASSIGN(MessagePump);
};

}  // namespace felicia

#endif  // FELICIA_CORE_PLATFORM_POSIX_MESSAGE_PUMP_H_