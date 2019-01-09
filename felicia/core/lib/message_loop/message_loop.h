// Copyright 2013 The Chromium Authors. All rights reserved.
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

#ifndef FELICIA_CORE_LIB_MESSAGE_LOOP_MESSAGE_LOOP_H_
#define FELICIA_CORE_LIB_MESSAGE_LOOP_MESSAGE_LOOP_H_

#include <functional>

#include "felicia/core/lib/threading/platform_thread_id.h"
#include "felicia/core/platform/message_pump.h"

namespace felicia {

class MessageLoopBase {
 public:
  MessageLoopBase();
  virtual ~MessageLoopBase();

  // Returns true if this is the active MessageLoop for the current thread.
  virtual bool IsBoundToCurrentThread() const = 0;

  // Returns the MessagePump owned by this MessageLoop if any.
  virtual MessagePump* GetMessagePump() const = 0;

  // Binds the MessageLoop to the current thread using |pump|.
  virtual void BindToCurrentThread(std::unique_ptr<MessagePump> pump) = 0;

 protected:
  friend class MessageLoop;
};

class MessageLoop {
 public:
  MessageLoop();
  virtual ~MessageLoop();

  MessageLoopBase* GetMessageLoopBase();

  // Returns true if this is the active MessageLoop for the current thread.
  bool IsBoundToCurrentThread() const;

 protected:
  // Configure various members and bind this message loop to the current thread.
  void BindToCurrentThread();

  // A raw pointer to the MessagePump handed-off to |backend_|.
  // Valid for the lifetime of |backend_|.
  MessagePump* pump_;

  // The actual implentation of the MessageLoop — either MessageLoopImpl or
  // SequenceManager-based.
  const std::unique_ptr<MessageLoopBase> backend_;

 private:
  std::unique_ptr<MessageLoopBase> CreateMessageLoopImpl();

  std::unique_ptr<MessagePump> CreateMessagePump();

  // Id of the thread this message loop is bound to. Initialized once when the
  // MessageLoop is bound to its thread and constant forever after.
  PlatformThreadId thread_id_ = kInvalidThreadId;

  DISALLOW_COPY_AND_ASSIGN(MessageLoop);
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_MESSAGE_LOOP_MESSAGE_LOOP_H_