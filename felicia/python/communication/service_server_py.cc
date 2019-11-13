// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/python/communication/service_server_py.h"

#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {

PyServiceServer::PyServiceServer(py::object server) {
  server_ = rpc::PyServerBridge(server);
}

void PyServiceServer::RequestRegister(const NodeInfo& node_info,
                                      const std::string& service,
                                      py::function py_callback) {
  StatusOnceCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }

  py::gil_scoped_release release;
  ServiceServer<EmptyService, rpc::PyServerBridge>::RequestRegister(
      node_info, service, std::move(callback));
}

void PyServiceServer::RequestUnregister(const NodeInfo& node_info,
                                        const std::string& service,
                                        py::function py_callback) {
  StatusOnceCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }

  py::gil_scoped_release release;
  ServiceServer<EmptyService, rpc::PyServerBridge>::RequestUnregister(
      node_info, service, std::move(callback));
}

void AddServiceServer(py::module& m) {
  py::class_<PyServiceServer>(m, "ServiceServer")
      .def(py::init<py::object>(), py::arg("server"))
      .def("is_registering", &PyServiceServer::IsRegistering)
      .def("is_registered", &PyServiceServer::IsRegistered)
      .def("is_unregistering", &PyServiceServer::IsUnregistering)
      .def("is_unregistered", &PyServiceServer::IsUnregistered)
      .def("request_register",
           [](PyServiceServer& self, const NodeInfo& node_info,
              const std::string& service) {
             self.RequestRegister(node_info, service, py::none());
           },
           py::arg("node_info"), py::arg("service"))
      .def("request_register",
           [](PyServiceServer& self, const NodeInfo& node_info,
              const std::string& service, py::function callback) {
             self.RequestRegister(node_info, service, callback);
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("callback").none(true))
      .def("request_unregister",
           [](PyServiceServer& self, const NodeInfo& node_info,
              const std::string& service) {
             self.RequestUnregister(node_info, service, py::none());
           },
           py::arg("node_info"), py::arg("service"))
      .def("request_unregister",
           [](PyServiceServer& self, const NodeInfo& node_info,
              const std::string& service, py::function callback) {
             self.RequestUnregister(node_info, service, callback);
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("callback").none(true));
}

}  // namespace felicia