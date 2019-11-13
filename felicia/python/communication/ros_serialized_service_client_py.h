// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#include "pybind11/pybind11.h"

#include "felicia/core/communication/service_client.h"
#include "felicia/python/rpc/ros_serialized_client_py.h"

namespace py = pybind11;

namespace felicia {

class PyRosSerializedServiceClient
    : public ServiceClient<rpc::PyRosSerializedClient> {
 public:
  explicit PyRosSerializedServiceClient(py::object ros_service);

  void RequestRegister(const NodeInfo& node_info, const std::string& service,
                       py::function py_on_connect_callback = py::none(),
                       py::function py_callback = py::none());

  void RequestUnregister(const NodeInfo& node_info, const std::string& service,
                         py::function py_callback = py::none());

  void Call(py::object py_request, py::object py_response,
            py::function py_callback);
};

void AddRosSerializedServiceClient(py::module& m);

}  // namespace felicia

#endif  // defined(HAS_ROS)