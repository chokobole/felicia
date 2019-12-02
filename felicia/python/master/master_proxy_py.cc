// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(FEL_PY_BINDING)

#include "felicia/python/master/master_proxy_py.h"

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/master/master_proxy.h"
#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {

Status PyMasterProxy::Start() {
  py::gil_scoped_release release;
  return MasterProxy::GetInstance().Start();
}

Status PyMasterProxy::Stop() {
  py::gil_scoped_release release;
  return MasterProxy::GetInstance().Stop();
}

void PyMasterProxy::RequestRegisterNode(py::function constructor,
                                        const NodeInfo& node_info,
                                        py::args args, py::kwargs kwargs) {
  py::object object = constructor(*args, **kwargs);
  object.inc_ref();
  NodeLifecycle* node = object.cast<NodeLifecycle*>();
  node->OnInit();

  py::gil_scoped_release release;
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  RegisterNodeRequest* request = new RegisterNodeRequest();
  NodeInfo* new_node_info = request->mutable_node_info();
  new_node_info->set_client_id(master_proxy.client_info_.id());
  new_node_info->set_name(node_info.name());
  RegisterNodeResponse* response = new RegisterNodeResponse();
  master_proxy.RegisterNodeAsync(
      request, response,
      base::BindOnce(&PyMasterProxy::OnRegisterNodeAsync,
                     base::Unretained(this), object, base::Owned(request),
                     base::Owned(response)));
}

void PyMasterProxy::OnRegisterNodeAsync(py::object object,
                                        const RegisterNodeRequest* request,
                                        RegisterNodeResponse* response,
                                        Status s) {
  NodeLifecycle* node = object.cast<NodeLifecycle*>();
  if (s.ok()) {
    std::unique_ptr<NodeInfo> node_info(response->release_node_info());
    node->OnDidCreate(std::move(*node_info));
  } else {
    Status new_status(s.error_code(),
                      base::StringPrintf("Failed to register node : %s",
                                         s.error_message().c_str()));
    node->OnError(std::move(new_status));
  }
}

void AddMasterProxy(py::module& m) {
  py::class_<PyMasterProxy>(m, "MasterProxy")
      .def("start", &PyMasterProxy::Start)
      .def("stop", &PyMasterProxy::Stop)
      .def("request_register_node", &PyMasterProxy::RequestRegisterNode,
           py::arg("constructor"), py::arg("node_info"));

  m.attr("master_proxy") = PyMasterProxy{};
}

}  // namespace felicia

#endif  // defined(FEL_PY_BINDING)