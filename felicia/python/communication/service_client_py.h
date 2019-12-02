// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_COMMUNICATION_SERVICE_CLIENT_PY_H_
#define FELICIA_PYTHON_COMMUNICATION_SERVICE_CLIENT_PY_H_

#if defined(FEL_PY_BINDING)

#include "pybind11/pybind11.h"

#include "felicia/core/communication/service_client.h"
#include "felicia/python/rpc/client_py.h"

namespace py = pybind11;

namespace felicia {

class PyServiceClient : public ServiceClient<rpc::PyClientBridge> {
 public:
  explicit PyServiceClient(py::object client);

  void RequestRegister(const NodeInfo& node_info, const std::string& service,
                       py::function py_on_connect_callback = py::none(),
                       py::function py_callback = py::none());

  void RequestUnregister(const NodeInfo& node_info, const std::string& service,
                         py::function py_callback = py::none());
};

void AddServiceClient(py::module& m);

}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)

#endif  // FELICIA_PYTHON_COMMUNICATION_SERVICE_CLIENT_PY_H_