// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_HOLDER_H_
#define FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_HOLDER_H_

#if defined(FEL_PY_BINDING)

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/macros.h"

namespace py = pybind11;

namespace felicia {

// TODO(chokobole): Remove this once c++ support lambda move capture.
template <typename Signature>
class PyOnceCallbackHolder;

template <typename R, typename... Args>
class PyOnceCallbackHolder<R(Args...)> {
 public:
  explicit PyOnceCallbackHolder(base::OnceCallback<R(Args...)> callback)
      : callback_(std::move(callback)) {}

  R Invoke(Args... args) {
    py::gil_scoped_release release;
    return std::move(callback_).Run(std::forward<Args>(args)...);
  }

 private:
  base::OnceCallback<R(Args...)> callback_;
};

template <typename Signature>
struct PyScopedOnceCallbackHolder;

template <typename R, typename... Args>
struct PyScopedOnceCallbackHolder<R(Args...)> {
  explicit PyScopedOnceCallbackHolder(
      PyOnceCallbackHolder<R(Args...)>* callback_holder)
      : callback_holder(callback_holder) {}
  ~PyScopedOnceCallbackHolder() { delete callback_holder; }

  PyOnceCallbackHolder<R(Args...)>* callback_holder;

  DISALLOW_COPY_AND_ASSIGN(PyScopedOnceCallbackHolder);
};

template <typename R, typename... Args>
std::function<R(Args...)> MakeLambdaFunc(
    base::OnceCallback<R(Args...)> callback) {
  PyOnceCallbackHolder<R(Args...)>* callback_holder =
      new PyOnceCallbackHolder<R(Args...)>(std::move(callback));
  return [callback_holder](Args... args) {
    PyScopedOnceCallbackHolder<R(Args...)> scoped_callback_holder(
        callback_holder);
    callback_holder->Invoke(std::forward<Args>(args)...);
  };
}

}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)

#endif  // FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_HOLDER_H_