// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/python/drivers/drivers_py.h"

#include "felicia/python/drivers/camera_py.h"

namespace felicia {

void AddDrivers(py::module& m) {
  py::module drivers = m.def_submodule("drivers");

  drivers::AddCamera(drivers);
}

}  // namespace felicia