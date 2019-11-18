// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/thread/main_thread.h"

#include <csignal>

namespace felicia {

namespace {

bool g_on_background = false;

void StopMainThread(int signal) {
  MainThread& main_thread = MainThread::GetInstance();
  main_thread.Stop();
}

}  // namespace

MainThread::MainThread() {
  if (g_on_background) {
    thread_ = std::make_unique<base::Thread>("MainThread");
  } else {
    message_loop_ =
        std::make_unique<base::MessageLoop>(base::MessageLoop::TYPE_IO);
    run_loop_ = std::make_unique<base::RunLoop>();
  }
}

MainThread::~MainThread() = default;

// static
void MainThread::SetBackground() { g_on_background = true; }

// static
MainThread& MainThread::GetInstance() {
  static base::NoDestructor<MainThread> main_thread;
  return *main_thread;
}

void MainThread::set_on_stop_callback(base::OnceClosure callback) {
  on_stop_callback_ = std::move(callback);
}

#if defined(OS_WIN)
void MainThread::InitCOM(bool use_mta) {
  scoped_com_initializer_.reset(use_mta
                                    ? new base::win::ScopedCOMInitializer(
                                          base::win::ScopedCOMInitializer::kMTA)
                                    : new base::win::ScopedCOMInitializer());
}
#endif

bool MainThread::IsBoundToCurrentThread() const {
  if (g_on_background) {
    return thread_->task_runner()->BelongsToCurrentThread();
  } else {
    return message_loop_->IsBoundToCurrentThread();
  }
}

bool MainThread::PostTask(const base::Location& from_here,
                          base::OnceClosure callback) {
  if (g_on_background) {
    return thread_->task_runner()->PostTask(from_here, std::move(callback));
  } else {
    return message_loop_->task_runner()->PostTask(from_here,
                                                  std::move(callback));
  }
}

bool MainThread::PostDelayedTask(const base::Location& from_here,
                                 base::OnceClosure callback,
                                 base::TimeDelta delay) {
  if (g_on_background) {
    return thread_->task_runner()->PostDelayedTask(from_here,
                                                   std::move(callback), delay);
  } else {
    return message_loop_->task_runner()->PostDelayedTask(
        from_here, std::move(callback), delay);
  }
}

void MainThread::Run() {
  if (g_on_background) return;
  RegisterSignals();
  run_loop_->Run();
}

void MainThread::RunBackground() {
  if (!g_on_background) return;
  thread_->StartWithOptions(
      base::Thread::Options{base::MessageLoop::TYPE_IO, 0});
}

void MainThread::Stop() {
  if (g_on_background) {
    thread_->Stop();
  } else {
    run_loop_->Quit();
  }
  if (!on_stop_callback_.is_null()) std::move(on_stop_callback_).Run();
}

void MainThread::RegisterSignals() {
  // To handle general case when POSIX ask the process to quit.
  std::signal(SIGTERM, &felicia::StopMainThread);
  // To handle Ctrl + C.
  std::signal(SIGINT, &felicia::StopMainThread);
#if defined(OS_POSIX)
  // To handle when the terminal is closed.
  std::signal(SIGHUP, &felicia::StopMainThread);
#endif
}

}  // namespace felicia