// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_PYTHON_RPC_CLIENT_PY_H_
#define FELICIA_PYTHON_RPC_CLIENT_PY_H_

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"

#include "felicia/core/rpc/client_interface.h"
#include "felicia/python/type_conversion/protobuf.h"
#include "felicia/python/type_conversion/util.h"

namespace py = pybind11;

namespace felicia {
namespace rpc {

class PyClientInterface : public ClientInterface {
 public:
  void Connect(const IPEndPoint& ip_endpoint, StatusOnceCallback callback) {
    NOTREACHED();
  }

  virtual void Connect(const IPEndPoint& ip_endpoint,
                       std::function<void(Status)> callback) = 0;
};

class PyClient : public PyClientInterface {
 public:
  using PyClientInterface::PyClientInterface;

  void Connect(const IPEndPoint& ip_endpoint,
               std::function<void(Status)> callback) override {
    FEL_OVERLOAD_INT_WITH_GIL(void, PyClientInterface, "connect", ip_endpoint,
                              callback);
    FEL_CALL_PURE_FUNCTION(PyClientInterface, "Connect");
  }

  Status Run() override { return Status::OK(); }

  Status Shutdown() override {
    FEL_OVERLOAD_INT_WITH_GIL(Status, PyClientInterface, "shutdown");
    FEL_CALL_PURE_FUNCTION(PyClientInterface, "Shutdown");
  }
};

class PyClientBridge {
 public:
  PyClientBridge();
  explicit PyClientBridge(py::object client);

  void set_service_info(const ServiceInfo& service_info);

  void Connect(const IPEndPoint& ip_endpoint, StatusOnceCallback callback);

  Status Run();

  Status Shutdown();

 private:
  py::object client_;
};

void AddClient(py::module& m);

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_CLIENT_PY_H_