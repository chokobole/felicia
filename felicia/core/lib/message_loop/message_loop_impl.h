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

#ifndef FELICIA_CORE_LIB_MESSAGE_LOOP_MESSAGE_LOOP_IMPL_H_
#define FELICIA_CORE_LIB_MESSAGE_LOOP_MESSAGE_LOOP_IMPL_H_

#include "absl/time/time.h"

#include "felicia/core/lib/base/compiler_specific.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/run_loop.h"
#include "felicia/core/lib/message_loop/message_loop.h"
#include "felicia/core/platform/message_pump.h"

namespace felicia {

class EXPORT MessageLoopImpl : public MessageLoopBase,
                               public MessagePump::Delegate,
                               public RunLoop::Delegate {
 public:
  // Create an unbound MessageLoopImpl implementation.
  // Pump will be created by owning MessageLoop and will be passed via
  // BindToCurrentThread.
  explicit MessageLoopImpl();

  ~MessageLoopImpl() override;

  bool IsBoundToCurrentThread() const override;
  MessagePump* GetMessagePump() const override;
  void BindToCurrentThread(std::unique_ptr<MessagePump> pump) override;

  bool IsCurrent() const;

  //----------------------------------------------------------------------------
 protected:
  std::unique_ptr<MessagePump> pump_;

 private:
  friend class MessageLoopCurrent;

  // RunLoop::Delegate:
  void Run() override;
  void Quit() override;
  void EnsureWorkScheduled() override;

  // MessagePump::Delegate methods:
  bool DoWork() override;
  bool DoDelayedWork(absl::Time* next_delayed_work_time) override;
  bool DoIdleWork() override;

  // Id of the thread this message loop is bound to. Initialized once when the
  // MessageLoop is bound to its thread and constant forever after.
  PlatformThreadId thread_id_ = kInvalidThreadId;

  DISALLOW_COPY_AND_ASSIGN(MessageLoopImpl);
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_MESSAGE_LOOP_MESSAGE_LOOP_IMPL_H_