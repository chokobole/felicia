// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#ifndef FELICIA_PYTHON_COMMUNICATION_ROS_SERIALIZED_SERVICE_SERVER_PY_H_
#define FELICIA_PYTHON_COMMUNICATION_ROS_SERIALIZED_SERVICE_SERVER_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/core/communication/service_server.h"
#include "felicia/python/rpc/ros_serialized_server_py.h"

namespace py = pybind11;

namespace felicia {

class PyRosSerializedServiceServer
    : public ServiceServer<EmptyService, rpc::PyRosSerializedServer> {
 public:
  explicit PyRosSerializedServiceServer(py::object service);

  void RequestRegister(const NodeInfo& node_info, const std::string& service,
                       py::function py_callback = py::none());

  void RequestUnregister(const NodeInfo& node_info, const std::string& service,
                         py::function py_callback = py::none());
};

void AddRosSerializedServiceServer(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_COMMUNICATION_ROS_SERIALIZED_SERVICE_SERVER_PY_H_

#endif  // defined(HAS_ROS)