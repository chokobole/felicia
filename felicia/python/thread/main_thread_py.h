// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_THREAD_MAIN_THREAD_PY_H_
#define FELICIA_PYTHON_THREAD_MAIN_THREAD_PY_H_

#include "pybind11/pybind11.h"

#include "third_party/chromium/build/build_config.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/master.pb.h"

namespace py = pybind11;

namespace felicia {

// This class is a wrapper for felicia::MainThread to open apis for python
// side.
class PyMainThread {
 public:
#if defined(OS_WIN)
  void InitCOM(bool use_mta);
#endif

  bool IsBoundToCurrentThread() const;

  bool PostTask(py::function callback);

  bool PostDelayedTask(py::function callback, base::TimeDelta delay);

  void Run();
  void Stop();

 private:
  friend void AddMainThread(py::module&);

  PyMainThread() = default;
};

void AddMainThread(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_THREAD_MAIN_THREAD_PY_H_