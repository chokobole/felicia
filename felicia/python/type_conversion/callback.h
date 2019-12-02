// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_H_
#define FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_H_

#if defined(FEL_PY_BINDING)

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"

#include "felicia/python/type_conversion/callback_forward.h"

namespace py = pybind11;

namespace felicia {

template <typename R, typename... Args>
class PyCallback<R(Args...)> {
 public:
  explicit PyCallback(const py::function& func)
      : func_(py::cast<std::function<R(Args...)>>(func)) {}

  R Invoke(Args... args) { return func_(std::forward<Args>(args)...); }

 private:
  std::function<R(Args...)> func_;
};

}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)

#endif  // FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_H_