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

#include "felicia/core/lib/message_loop/message_loop.h"

#include "absl/memory/memory.h"

#include "felicia/core/lib/base/no_destructor.h"
#include "felicia/core/lib/message_loop/message_loop_current.h"
#include "felicia/core/lib/message_loop/message_loop_impl.h"
#include "felicia/core/platform/platform_thread.h"

namespace felicia {

MessageLoopBase::MessageLoopBase() = default;
MessageLoopBase::~MessageLoopBase() = default;

MessageLoop::MessageLoop() : pump_(nullptr), backend_(CreateMessageLoopImpl()) {
  BindToCurrentThread();
}

MessageLoop::~MessageLoop() {
  // If |pump_| is non-null, this message loop has been bound and should be the
  // current one on this thread. Otherwise, this loop is being destructed before
  // it was bound to a thread, so a different message loop (or no loop at all)
  // may be current.
  DCHECK((pump_ && IsBoundToCurrentThread()) ||
         (!pump_ && !IsBoundToCurrentThread()));
}

MessageLoopBase* MessageLoop::GetMessageLoopBase() { return backend_.get(); }

bool MessageLoop::IsBoundToCurrentThread() const {
  return backend_->IsBoundToCurrentThread();
}

void MessageLoop::BindToCurrentThread() {
  thread_id_ = PlatformThread::CurrentId();

  DCHECK(!pump_);

  std::unique_ptr<MessagePump> pump = CreateMessagePump();
  pump_ = pump.get();

  DCHECK(!MessageLoopCurrent::IsSet())
      << "should only have one message loop per thread";

  backend_->BindToCurrentThread(std::move(pump));
}

std::unique_ptr<MessageLoopBase> MessageLoop::CreateMessageLoopImpl() {
  return absl::make_unique<MessageLoopImpl>();
}

std::unique_ptr<MessagePump> MessageLoop::CreateMessagePump() {
  return absl::make_unique<MessagePump>();
}

}  // namespace felicia