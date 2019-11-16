// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_THREAD_MAIN_THREAD_H_
#define FELICIA_CORE_THREAD_MAIN_THREAD_H_

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/message_loop/message_loop.h"
#include "third_party/chromium/base/no_destructor.h"
#include "third_party/chromium/base/run_loop.h"
#include "third_party/chromium/base/threading/thread.h"
#if defined(OS_WIN)
#include "third_party/chromium/base/win/scoped_com_initializer.h"
#endif

#include "felicia/core/lib/base/export.h"

namespace felicia {

class FEL_EXPORT MainThread {
 public:
  static void SetBackground();
  static MainThread& GetInstance();

  void set_on_stop_callback(base::OnceClosure callback);
#if defined(OS_WIN)
  void InitCOM(bool use_mta);
#endif

  bool IsBoundToCurrentThread() const;

  bool PostTask(const base::Location& from_here, base::OnceClosure callback);

  bool PostDelayedTask(const base::Location& from_here,
                       base::OnceClosure callback, base::TimeDelta delay);

  void Run();
  void RunBackground();
  void Stop();

 private:
  friend class base::NoDestructor<MainThread>;

  MainThread();
  ~MainThread();

  void RegisterSignals();

  std::unique_ptr<base::MessageLoop> message_loop_;
  std::unique_ptr<base::RunLoop> run_loop_;
  std::unique_ptr<base::Thread> thread_;

  base::OnceClosure on_stop_callback_;
#if defined(OS_WIN)
  std::unique_ptr<base::win::ScopedCOMInitializer> scoped_com_initializer_;
#endif

  DISALLOW_COPY_AND_ASSIGN(MainThread);
};

}  // namespace felicia

#endif  // FELICIA_CORE_THREAD_MAIN_THREAD_H_