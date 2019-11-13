// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/python/rpc/server_py.h"

#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {
namespace rpc {

PyServerBridge::PyServerBridge() = default;

PyServerBridge::PyServerBridge(py::object server) : server_(server) {}

ChannelDef PyServerBridge::channel_def() const {
  PyServer* py_server = server_.cast<PyServer*>();
  return py_server->channel_def();
}

void PyServerBridge::set_service_info(const ServiceInfo& service_info) {
  PyServer* py_server = server_.cast<PyServer*>();
  return py_server->set_service_info(service_info);
}

Status PyServerBridge::Start() {
  PyServer* py_server = server_.cast<PyServer*>();
  return py_server->Start();
}

Status PyServerBridge::Run() {
  PyServer* py_server = server_.cast<PyServer*>();
  return py_server->Run();
}

Status PyServerBridge::Shutdown() {
  PyServer* py_server = server_.cast<PyServer*>();
  return py_server->Shutdown();
}

std::string PyServerBridge::GetServiceTypeName() const {
  PyServer* py_server = server_.cast<PyServer*>();
  return py_server->GetServiceTypeName();
}

void AddServer(py::module& m) {
  py::class_<ServerInterface, PyServer>(m, "_Server")
      .def(py::init<>())
      .def("ConfigureServerAddress", &ServerInterface::ConfigureServerAddress);
}

}  // namespace rpc
}  // namespace felicia