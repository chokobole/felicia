// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(FEL_PY_BINDING)
#if defined(HAS_ROS)

#include <utility>

#include "felicia/python/rpc/ros_serialized_server_py.h"

namespace felicia {
namespace rpc {

PyRosSerializedServer::PyRosSerializedServer() = default;

PyRosSerializedServer::PyRosSerializedServer(py::object service)
    : Server<PyRosSerializedServiceBridge>(
          std::make_unique<PyRosSerializedServiceBridge>(service)) {}

}  // namespace rpc
}  // namespace felicia

#endif  // defined(HAS_ROS)
#endif  // defined(FEL_PY_BINDING)