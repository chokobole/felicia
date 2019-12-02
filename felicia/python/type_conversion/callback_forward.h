// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_FORWARD_H_
#define FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_FORWARD_H_

#if defined(FEL_PY_BINDING)

#include "felicia/core/lib/error/status.h"

namespace felicia {

template <typename Signature>
class PyCallback;

using PyStatusCallback = PyCallback<void(Status)>;
using PyClosure = PyCallback<void()>;

}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)

#endif  // FELICIA_PYTHON_TYPE_CONVERSION_CALLBACK_FORWARD_H_