// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/python/thread/main_thread_py.h"

#include "felicia/core/thread/main_thread.h"
#include "felicia/python/type_conversion/callback.h"

namespace felicia {

#if defined(OS_WIN)
void PyMainThread::InitCOM(bool use_mta) {
  py::gil_scoped_release release;
  return MainThread::GetInstance().InitCOM(use_mta);
}
#endif

bool PyMainThread::IsBoundToCurrentThread() const {
  py::gil_scoped_release release;
  return MainThread::GetInstance().IsBoundToCurrentThread();
}

bool PyMainThread::PostTask(py::function callback) {
  PyClosure* closure = new PyClosure(callback);
  py::gil_scoped_release release;
  MainThread& main_thread = MainThread::GetInstance();
  return main_thread.PostTask(
      FROM_HERE, base::BindOnce(&PyClosure::Invoke, base::Owned(closure)));
}

bool PyMainThread::PostDelayedTask(py::function callback,
                                   base::TimeDelta delay) {
  PyClosure* closure = new PyClosure(callback);
  py::gil_scoped_release release;
  MainThread& main_thread = MainThread::GetInstance();
  return main_thread.PostDelayedTask(
      FROM_HERE, base::BindOnce(&PyClosure::Invoke, base::Owned(closure)),
      delay);
}

void PyMainThread::Run() {
  py::gil_scoped_release release;
  {
    py::gil_scoped_acquire acquire;
    acquire.inc_ref();
  }
  MainThread::GetInstance().Run();
}

void PyMainThread::Stop() {
  py::gil_scoped_release release;
  MainThread::GetInstance().Stop();
}

void AddMainThread(py::module& m) {
  py::class_<PyMainThread>(m, "MainThread")
#if defined(OS_WIN)
      .def("init_com", &PyMainThread::InitCOM, py::arg("use_mta"))
#endif
      .def("is_bound_to_current_thread", &PyMainThread::IsBoundToCurrentThread)
      .def("post_task", &PyMainThread::PostTask, py::arg("callback"))
      .def("post_delayed_task", &PyMainThread::PostDelayedTask,
           py::arg("callback"), py::arg("delay"))
      .def("run", &PyMainThread::Run)
      .def("stop", &PyMainThread::Stop);

  m.attr("main_thread") = PyMainThread{};
}

}  // namespace felicia