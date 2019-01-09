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

#ifndef FELICIA_CORE_LIB_BASE_RUN_LOOP_H_
#define FELICIA_CORE_LIB_BASE_RUN_LOOP_H_

#include <utility>
#include <vector>

#include "felicia/core/lib/base/compiler_specific.h"
#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/base/logging.h"
#include "felicia/core/lib/base/macros.h"
#include "felicia/core/lib/base/platform.h"

namespace felicia {

class EXPORT RunLoop {
 public:
  RunLoop();
  ~RunLoop();

  // Run the current RunLoop::Delegate. This blocks until Quit is called. Before
  // calling Run, be sure to grab the QuitClosure in order to stop the
  // RunLoop::Delegate asynchronously.
  void Run();

  void Quit();

  // A RunLoop::Delegate is a generic interface that allows RunLoop to be
  // separate from the underlying implementation of the message loop for this
  // thread. It holds private state used by RunLoops on its associated thread.
  // One and only one RunLoop::Delegate must be registered on a given thread
  // via RunLoop::RegisterDelegateForCurrentThread() before RunLoop instances
  // and RunLoop static methods can be used on it.
  class EXPORT Delegate {
   public:
    Delegate();
    virtual ~Delegate();

    // Used by RunLoop to inform its Delegate to Run/Quit. Implementations are
    // expected to keep on running synchronously from the Run() call until the
    // eventual matching Quit() call. Upon receiving a Quit() call it should
    // return from the Run() call as soon as possible without executing
    // remaining tasks/messages. Run() calls can nest in which case each Quit()
    // call should result in the topmost active Run() call returning. The only
    // other trigger for Run() to return is the
    // |should_quit_when_idle_callback_| which the Delegate should probe before
    // sleeping when it becomes idle. |application_tasks_allowed| is true if
    // this is the first Run() call on the stack or it was made from a nested
    // RunLoop of Type::kNestableTasksAllowed (otherwise this Run() level should
    // only process system tasks).
    virtual void Run() = 0;
    virtual void Quit() = 0;

    // Invoked right before a RunLoop enters a nested Run() call on this
    // Delegate iff this RunLoop is of type kNestableTasksAllowed. The Delegate
    // should ensure that the upcoming Run() call will result in processing
    // application tasks queued ahead of it without further probing. e.g.
    // message pumps on some platforms, like Mac, need an explicit request to
    // process application tasks when nested, otherwise they'll only wait for
    // system messages.
    virtual void EnsureWorkScheduled() = 0;

   protected:
    // Returns the result of this Delegate's |should_quit_when_idle_callback_|.
    // "protected" so it can be invoked only by the Delegate itself.
    bool ShouldQuitWhenIdle();

   private:
    // While the state is owned by the Delegate subclass, only RunLoop can use
    // it.
    friend class RunLoop;

    // True once this Delegate is bound to a thread via
    // RegisterDelegateForCurrentThread().
    bool bound_ = false;

    DISALLOW_COPY_AND_ASSIGN(Delegate);
  };

  // Registers |delegate| on the current thread. Must be called once and only
  // once per thread before using RunLoop methods on it. |delegate| is from then
  // on forever bound to that thread (including its destruction).
  static void RegisterDelegateForCurrentThread(Delegate* delegate);

 private:
  // Return false to abort the Run.
  bool BeforeRun();
  void AfterRun();

  // A copy of RunLoop::Delegate for the thread driven by tis RunLoop for quick
  // access without using TLS (also allows access to state from another sequence
  // during Run(), ref. |sequence_checker_| below).
  Delegate* delegate_;

#if DCHECK_IS_ON()
  bool run_called_ = false;
#endif

  bool quit_called_ = false;
  bool running_ = false;

  DISALLOW_COPY_AND_ASSIGN(RunLoop);
};

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_BASE_RUN_LOOP_H_
