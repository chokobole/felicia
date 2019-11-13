// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_DRIVERS_DRIVERS_PY_H_
#define FELICIA_PYTHON_DRIVERS_DRIVERS_PY_H_

#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace felicia {

void AddDrivers(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_DRIVERS_DRIVERS_PY_H_