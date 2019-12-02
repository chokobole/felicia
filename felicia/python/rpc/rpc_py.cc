// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(FEL_PY_BINDING)

#include "felicia/python/rpc/rpc_py.h"

#include "felicia/python/rpc/client_py.h"
#include "felicia/python/rpc/ros_serialized_service_py.h"
#include "felicia/python/rpc/server_py.h"

namespace felicia {

void AddRpc(py::module& m) {
  py::module rpc = m.def_submodule("rpc");

  rpc::AddClient(rpc);
  rpc::AddServer(rpc);
#if defined(HAS_ROS)
  rpc::AddRosSerializedService(rpc);
#endif
}

}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)