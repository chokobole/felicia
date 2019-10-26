#include "felicia/python/master/master_proxy_py.h"

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/master/master_proxy.h"
#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {

// static
Status PyMasterProxy::Start() { return MasterProxy::GetInstance().Start(); }

// static
Status PyMasterProxy::Stop() { return MasterProxy::GetInstance().Stop(); }

// static
void PyMasterProxy::Run() {
  py::gil_scoped_release release;
  {
    py::gil_scoped_acquire acquire;
    acquire.inc_ref();
  }
  MasterProxy::GetInstance().Run();
}

// static
bool PyMasterProxy::IsBoundToCurrentThread() {
  return MasterProxy::GetInstance().IsBoundToCurrentThread();
}

// static
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
      base::BindOnce(&PyMasterProxy::OnRegisterNodeAsync, object,
                     base::Owned(request), base::Owned(response)));
}

// static
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
      .def_static("start", &PyMasterProxy::Start)
      .def_static("stop", &PyMasterProxy::Stop)
      .def_static("run", &PyMasterProxy::Run)
      // .def_static("is_bound_to_current_thread",
      // &PyMasterProxy::IsBoundToCurrentThread)
      .def_static("request_register_node", &PyMasterProxy::RequestRegisterNode,
                  py::arg("constructor"), py::arg("node_info"))
      .def_static("post_task",
                  [](py::function callback) {
                    PyClosure* closure = new PyClosure(callback);
                    py::gil_scoped_release release;
                    MasterProxy& master_proxy = MasterProxy::GetInstance();
                    master_proxy.PostTask(FROM_HERE,
                                          base::BindOnce(&PyClosure::Invoke,
                                                         base::Owned(closure)));
                  },
                  py::arg("callback"))
      .def_static(
          "post_delayed_task",
          [](py::function callback, base::TimeDelta delay) {
            PyClosure* closure = new PyClosure(callback);
            py::gil_scoped_release release;
            MasterProxy& master_proxy = MasterProxy::GetInstance();
            master_proxy.PostDelayedTask(
                FROM_HERE,
                base::BindOnce(&PyClosure::Invoke, base::Owned(closure)),
                delay);
          },
          py::arg("callback"), py::arg("delay"));
}

}  // namespace felicia