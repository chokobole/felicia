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

#include "felicia/core/platform/message_pump.h"

#include <unistd.h>

#include <utility>

#include "absl/time/time.h"

#include "felicia/core/lib/base/auto_reset.h"
#include "felicia/core/lib/base/logging.h"
#include "felicia/core/platform/file_util.h"
#include "felicia/core/platform/posix/eintr_wrapper.h"

// Lifecycle of struct event
// Libevent uses two main data structures:
// struct event_base (of which there is one per message pump), and
// struct event (of which there is roughly one per socket).
// The socket's struct event is created in
// MessagePump::WatchFileDescriptor(),
// is owned by the FdWatchController, and is destroyed in
// StopWatchingFileDescriptor().
// It is moved into and out of lists in struct event_base by
// the libevent functions event_add() and event_del().
//
// TODO(dkegel):
// At the moment bad things happen if a FdWatchController
// is active after its MessagePump has been destroyed.
// See MessageLoopTest.FdWatchControllerOutlivesMessageLoop
// Not clear yet whether that situation occurs in practice,
// but if it does, we need to fix it.

namespace felicia {

MessagePump::FdWatchController::FdWatchController(const Location& from_here)
    : FdWatchControllerInterface(from_here) {}

MessagePump::FdWatchController::~FdWatchController() {
  if (event_) {
    StopWatchingFileDescriptor();
  }
  if (was_destroyed_) {
    DCHECK(!*was_destroyed_);
    *was_destroyed_ = true;
  }
}

bool MessagePump::FdWatchController::StopWatchingFileDescriptor() {
  std::unique_ptr<event> e = ReleaseEvent();
  if (!e) return true;

  // event_del() is a no-op if the event isn't active.
  int rv = event_del(e.get());
  pump_ = nullptr;
  watcher_ = nullptr;
  return (rv == 0);
}

void MessagePump::FdWatchController::Init(std::unique_ptr<event> e) {
  DCHECK(e);
  DCHECK(!event_);

  event_ = std::move(e);
}

std::unique_ptr<event> MessagePump::FdWatchController::ReleaseEvent() {
  return std::move(event_);
}

void MessagePump::FdWatchController::OnFileCanReadWithoutBlocking(
    int fd, MessagePump* pump) {
  // Since OnFileCanWriteWithoutBlocking() gets called first, it can stop
  // watching the file descriptor.
  if (!watcher_) return;
  watcher_->OnFileCanReadWithoutBlocking(fd);
}

void MessagePump::FdWatchController::OnFileCanWriteWithoutBlocking(
    int fd, MessagePump* pump) {
  DCHECK(watcher_);
  watcher_->OnFileCanWriteWithoutBlocking(fd);
}

MessagePump::MessagePump()
    : keep_running_(true),
      in_run_(false),
      processed_io_events_(false),
      event_base_(event_base_new()),
      wakeup_pipe_in_(-1),
      wakeup_pipe_out_(-1) {
  if (!Init()) NOTREACHED();
}

MessagePump::~MessagePump() {
  DCHECK(wakeup_event_);
  DCHECK(event_base_);
  event_del(wakeup_event_);
  delete wakeup_event_;
  if (wakeup_pipe_in_ >= 0) {
    if (IGNORE_EINTR(close(wakeup_pipe_in_)) < 0) DPLOG(ERROR) << "close";
  }
  if (wakeup_pipe_out_ >= 0) {
    if (IGNORE_EINTR(close(wakeup_pipe_out_)) < 0) DPLOG(ERROR) << "close";
  }
  event_base_free(event_base_);
}

bool MessagePump::WatchFileDescriptor(int fd, bool persistent, int mode,
                                      FdWatchController* controller,
                                      FdWatcher* delegate) {
  DCHECK_GE(fd, 0);
  DCHECK(controller);
  DCHECK(delegate);
  DCHECK(mode == WATCH_READ || mode == WATCH_WRITE || mode == WATCH_READ_WRITE);

  int event_mask = persistent ? EV_PERSIST : 0;
  if (mode & WATCH_READ) {
    event_mask |= EV_READ;
  }
  if (mode & WATCH_WRITE) {
    event_mask |= EV_WRITE;
  }

  std::unique_ptr<event> evt(controller->ReleaseEvent());
  if (!evt) {
    // Ownership is transferred to the controller.
    evt.reset(new event);
  } else {
    // Make sure we don't pick up any funky internal libevent masks.
    int old_interest_mask = evt->ev_events & (EV_READ | EV_WRITE | EV_PERSIST);

    // Combine old/new event masks.
    event_mask |= old_interest_mask;

    // Must disarm the event before we can reuse it.
    event_del(evt.get());

    // It's illegal to use this function to listen on 2 separate fds with the
    // same |controller|.
    if (EVENT_FD(evt.get()) != fd) {
      NOTREACHED() << "FDs don't match" << EVENT_FD(evt.get()) << "!=" << fd;
      return false;
    }
  }

  // Set current interest mask and message pump for this event.
  event_set(evt.get(), fd, event_mask, OnNotification, controller);

  // Tell libevent which message pump this socket will belong to when we add it.
  if (event_base_set(event_base_, evt.get())) {
    DPLOG(ERROR) << "event_base_set(fd=" << EVENT_FD(evt.get()) << ")";
    return false;
  }

  // Add this socket to the list of monitored sockets.
  if (event_add(evt.get(), nullptr)) {
    DPLOG(ERROR) << "event_add failed(fd=" << EVENT_FD(evt.get()) << ")";
    return false;
  }

  controller->Init(std::move(evt));
  controller->set_watcher(delegate);
  controller->set_pump(this);
  return true;
}

// Tell libevent to break out of inner loop.
static void timer_callback(int fd, short events, void* context) {
  event_base_loopbreak((struct event_base*)context);
}

// Reentrant!
void MessagePump::Run(Delegate* delegate) {
  AutoReset<bool> auto_reset_keep_running(&keep_running_, true);
  AutoReset<bool> auto_reset_in_run(&in_run_, true);

  // event_base_loopexit() + EVLOOP_ONCE is leaky, see http://crbug.com/25641.
  // Instead, make our own timer and reuse it on each call to event_base_loop().
  std::unique_ptr<event> timer_event(new event);

  for (;;) {
    bool did_work = delegate->DoWork();
    if (!keep_running_) break;

    event_base_loop(event_base_, EVLOOP_NONBLOCK);
    did_work |= processed_io_events_;
    processed_io_events_ = false;
    if (!keep_running_) break;

    did_work |= delegate->DoDelayedWork(&delayed_work_time_);
    if (!keep_running_) break;

    if (did_work) continue;

    did_work = delegate->DoIdleWork();
    if (!keep_running_) break;

    if (did_work) continue;

    // EVLOOP_ONCE tells libevent to only block once,
    // but to service all pending events when it wakes up.
    if (delayed_work_time_ == absl::Time()) {
      event_base_loop(event_base_, EVLOOP_ONCE);
    } else {
      absl::Duration delay = delayed_work_time_ - absl::Now();
      if (delay > absl::Duration()) {
        struct timeval poll_tv(absl::ToTimeval(delay));
        event_set(timer_event.get(), -1, 0, timer_callback, event_base_);
        event_base_set(event_base_, timer_event.get());
        event_add(timer_event.get(), &poll_tv);
        event_base_loop(event_base_, EVLOOP_ONCE);
        event_del(timer_event.get());
      } else {
        // It looks like delayed_work_time_ indicates a time in the past, so we
        // need to call DoDelayedWork now.
        delayed_work_time_ = absl::Time();
      }
    }

    if (!keep_running_) break;
  }
}

void MessagePump::Quit() {
  DCHECK(in_run_) << "Quit was called outside of Run!";
  // Tell both libevent and Run that they should break out of their loops.
  keep_running_ = false;
  ScheduleWork();
}

void MessagePump::ScheduleWork() {
  // Tell libevent (in a threadsafe way) that it should break out of its loop.
  char buf = 0;
  int nwrite = HANDLE_EINTR(write(wakeup_pipe_in_, &buf, 1));
  DPCHECK(nwrite == 1 || errno == EAGAIN) << "nwrite:" << nwrite;
}

void MessagePump::ScheduleDelayedWork(const absl::Time delayed_work_time) {
  // We know that we can't be blocked on Wait right now since this method can
  // only be called on the same thread as Run, so we only need to update our
  // record of how long to sleep when we do sleep.
  delayed_work_time_ = delayed_work_time;
}

bool MessagePump::Init() {
  int fds[2];
  if (!CreateLocalNonBlockingPipe(fds)) {
    DPLOG(ERROR) << "pipe creation failed";
    return false;
  }
  wakeup_pipe_out_ = fds[0];
  wakeup_pipe_in_ = fds[1];

  wakeup_event_ = new event;
  event_set(wakeup_event_, wakeup_pipe_out_, EV_READ | EV_PERSIST, OnWakeup,
            this);
  event_base_set(event_base_, wakeup_event_);

  if (event_add(wakeup_event_, nullptr)) return false;
  return true;
}

// static
void MessagePump::OnNotification(int fd, short flags, void* context) {
  FdWatchController* controller = static_cast<FdWatchController*>(context);
  DCHECK(controller);

  MessagePump* pump = controller->pump();
  pump->processed_io_events_ = true;

  if ((flags & (EV_READ | EV_WRITE)) == (EV_READ | EV_WRITE)) {
    // Both callbacks will be called. It is necessary to check that |controller|
    // is not destroyed.
    bool controller_was_destroyed = false;
    controller->was_destroyed_ = &controller_was_destroyed;
    controller->OnFileCanWriteWithoutBlocking(fd, pump);
    if (!controller_was_destroyed)
      controller->OnFileCanReadWithoutBlocking(fd, pump);
    if (!controller_was_destroyed) controller->was_destroyed_ = nullptr;
  } else if (flags & EV_WRITE) {
    controller->OnFileCanWriteWithoutBlocking(fd, pump);
  } else if (flags & EV_READ) {
    controller->OnFileCanReadWithoutBlocking(fd, pump);
  }
}

// Called if a byte is received on the wakeup pipe.
// static
void MessagePump::OnWakeup(int socket, short flags, void* context) {
  MessagePump* that = static_cast<MessagePump*>(context);
  DCHECK(that->wakeup_pipe_out_ == socket);

  // Remove and discard the wakeup byte.
  char buf;
  int nread = HANDLE_EINTR(read(socket, &buf, 1));
  DCHECK_EQ(nread, 1);
  that->processed_io_events_ = true;
  // Tell libevent to break out of inner loop.
  event_base_loopbreak(that->event_base_);
}

}  // namespace felicia
