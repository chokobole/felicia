// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_DRIVERS_CAMERA_PY_H_
#define FELICIA_PYTHON_DRIVERS_CAMERA_PY_H_

#if defined(FEL_PY_BINDING)

#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace felicia {
namespace drivers {

void AddCamera(py::module& m);

}  // namespace drivers
}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)

#endif  // FELICIA_PYTHON_DRIVERS_CAMERA_PY_H_