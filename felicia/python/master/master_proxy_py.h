// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_MASTER_MASTER_PROXY_PY_H_
#define FELICIA_PYTHON_MASTER_MASTER_PROXY_PY_H_

#if defined(FEL_PY_BINDING)

#include "pybind11/pybind11.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/protobuf/master.pb.h"

namespace py = pybind11;

namespace felicia {

// This class is a wrapper for felicia::MasterProxy to open apis for python
// side.
class PyMasterProxy {
 public:
  Status Start();
  Status Stop();

  void RequestRegisterNode(py::function constructor, const NodeInfo& node_info,
                           py::args args, py::kwargs kwargs);

 private:
  friend void AddMasterProxy(py::module&);

  PyMasterProxy() = default;

  void OnRegisterNodeAsync(py::object object,
                           const RegisterNodeRequest* request,
                           RegisterNodeResponse* response, Status s);
};

void AddMasterProxy(py::module& m);

}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)

#endif  // FELICIA_PYTHON_MASTER_MASTER_PROXY_PY_H_