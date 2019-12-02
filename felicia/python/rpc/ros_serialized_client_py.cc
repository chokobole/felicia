// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(FEL_PY_BINDING)
#if defined(HAS_ROS)

#include "felicia/python/rpc/ros_serialized_client_py.h"

#include "felicia/python/message/message_util.h"

namespace felicia {
namespace rpc {

PyRosSerializedClient::PyRosSerializedClient() = default;

PyRosSerializedClient::PyRosSerializedClient(py::object ros_service) {
  service_type_name_ =
      GetMessageTypeNameFromPyObject(ros_service, TopicInfo::ROS);
  service_md5sum_ = GetMessageMD5SumFromPyObject(ros_service, TopicInfo::ROS);
}

void AddRosSerializedClient(py::module& m) {
  py::class_<PyRosSerializedClient>(m, "RosClient")
      .def(py::init<py::object>(), py::arg("ros_service"));
}

}  // namespace rpc
}  // namespace felicia

#endif  // defined(HAS_ROS)
#endif  // defined(FEL_PY_BINDING)