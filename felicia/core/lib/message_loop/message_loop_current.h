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

#ifndef FELICIA_CORE_LIB_MESSAGE_LOOP_MESSAGE_LOOP_CURRENT_H_
#define FELICIA_CORE_LIB_MESSAGE_LOOP_MESSAGE_LOOP_CURRENT_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/message_loop/message_loop.h"
#include "felicia/core/platform/message_pump.h"

namespace felicia {

// MessageLoopCurrent is a proxy to the public interface of the MessageLoop
// bound to the thread it's obtained on.
//
class EXPORT MessageLoopCurrent {
 public:
  // MessageLoopCurrent is effectively just a disguised pointer and is fine to
  // copy/move around.
  MessageLoopCurrent(const MessageLoopCurrent& other) = default;
  MessageLoopCurrent(MessageLoopCurrent&& other) = default;
  MessageLoopCurrent& operator=(const MessageLoopCurrent& other) = default;

  bool operator==(const MessageLoopCurrent& other) const;

  // Returns a proxy object to interact with the MessageLoop running the
  // current thread. It must only be used on the thread it was obtained.
  static MessageLoopCurrent Get();

  // Returns true if the current thread is running a MessageLoop. Prefer this to
  // verifying the boolean value of Get() (so that Get() can ultimately DCHECK
  // it's only invoked when IsSet()).
  static bool IsSet();

  // Allow MessageLoopCurrent to be used like a pointer to support the many
  // callsites that used MessageLoop::current() that way when it was a
  // MessageLoop*.
  MessageLoopCurrent* operator->() { return this; }
  explicit operator bool() const { return !!current_; }

  // Returns true if this is the active MessageLoop for the current thread.
  bool IsBoundToCurrentThread() const;

  bool WatchFileDescriptor(int fd, bool persistent, MessagePump::Mode mode,
                           MessagePump::FdWatchController* controller,
                           MessagePump::FdWatcher* delegate);

 protected:
  friend class MessageLoopImpl;

  // Binds |current| to the current thread. It will from then on be the
  // MessageLoop driven by MessageLoopCurrent on this thread. This is only meant
  // to be invoked by the MessageLoop itself.
  static void BindToCurrentThreadInternal(MessageLoopBase* current);

  explicit MessageLoopCurrent(MessageLoopBase* current) : current_(current) {}

  MessageLoopBase* current_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_MESSAGE_LOOP_MESSAGE_LOOP_CURRENT_H_