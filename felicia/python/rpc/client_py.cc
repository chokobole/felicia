// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(FEL_PY_BINDING)

#include "felicia/python/rpc/client_py.h"

#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/callback_holder.h"
#include "felicia/python/type_conversion/protobuf.h"
#include "felicia/python/type_conversion/util.h"

namespace felicia {
namespace rpc {

PyClientBridge::PyClientBridge() = default;

PyClientBridge::PyClientBridge(py::object client) : client_(client) {}

void PyClientBridge::set_service_info(const ServiceInfo& service_info) {
  PyClient* py_client = client_.cast<PyClient*>();
  py_client->set_service_info(service_info);
}

void PyClientBridge::Connect(const IPEndPoint& ip_endpoint,
                             StatusOnceCallback callback) {
  PyClient* py_client = client_.cast<PyClient*>();
  auto py_callback = MakeLambdaFunc(std::move(callback));
  py_client->Connect(ip_endpoint, py_callback);
}

Status PyClientBridge::Run() {
  PyClient* py_client = client_.cast<PyClient*>();
  return py_client->Run();
}

Status PyClientBridge::Shutdown() {
  PyClient* py_client = client_.cast<PyClient*>();
  return py_client->Shutdown();
}

void AddClient(py::module& m) {
  py::class_<PyClientInterface, PyClient>(m, "_Client")
      .def(py::init<>())
      .def("service_info", &PyClientInterface::service_info);
}

}  // namespace rpc
}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)