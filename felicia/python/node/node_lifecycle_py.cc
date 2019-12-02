// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(FEL_PY_BINDING)

#include "felicia/python/node/node_lifecycle_py.h"

#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {

void AddNodeLifecycle(py::module& m) {
  py::class_<NodeLifecycle, PyNodeLifecycle>(m, "NodeLifecycle")
      .def(py::init<>());
}

}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)