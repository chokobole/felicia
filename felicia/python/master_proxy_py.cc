#include "felicia/python/master_proxy_py.h"

#include <csignal>

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"
#include "third_party/chromium/build/build_config.h"

#include "felicia/core/master/master_proxy.h"
#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(felicia::NodeInfo, NodeInfo,
                           felicia.core.protobuf.master_data_pb2)

namespace felicia {

#ifdef OS_POSIX
namespace {

void Shutdown(int signal) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.Stop();
}

}  // namespace
#endif

// static
Status PyMasterProxy::Start() { return MasterProxy::GetInstance().Start(); }

// static
Status PyMasterProxy::Stop() { return MasterProxy::GetInstance().Stop(); }

// static
void PyMasterProxy::Run() {
#ifdef OS_POSIX
  // To handle general case when POSIX ask the process to quit.
  std::signal(SIGTERM, &felicia::Shutdown);
  // To handle Ctrl + C.
  std::signal(SIGINT, &felicia::Shutdown);
  // To handle when the terminal is closed.
  std::signal(SIGHUP, &felicia::Shutdown);
#endif
  MasterProxy::GetInstance().Run();
}

// static
void PyMasterProxy::RequestRegisterNode(py::function constructor,
                                        const NodeInfo& node_info,
                                        py::args args, py::kwargs kwargs) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();

  RegisterNodeRequest* request = new RegisterNodeRequest();
  NodeInfo* new_node_info = request->mutable_node_info();
  new_node_info->set_client_id(master_proxy.client_info_.id());
  new_node_info->set_name(node_info.name());
  RegisterNodeResponse* response = new RegisterNodeResponse();

  py::object object = constructor(*args, **kwargs);
  object.inc_ref();
  NodeLifecycle* node = object.cast<NodeLifecycle*>();

  py::gil_scoped_release release;
  node->OnInit();
  master_proxy.RegisterNodeAsync(
      request, response,
      base::BindOnce(&PyMasterProxy::OnRegisterNodeAsync, object, request,
                     response));
}

// static
void PyMasterProxy::OnRegisterNodeAsync(py::object object,
                                        RegisterNodeRequest* request,
                                        RegisterNodeResponse* response,
                                        const Status& s) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  if (!master_proxy.IsBoundToCurrentThread()) {
    master_proxy.PostTask(
        FROM_HERE,
        base::BindOnce(&PyMasterProxy::OnRegisterNodeAsync, object,
                       base::Owned(request), base::Owned(response), s));
    return;
  }

  if (!s.ok()) {
    Status new_status(s.error_code(),
                      base::StringPrintf("Failed to register node : %s",
                                         s.error_message().c_str()));
    NodeLifecycle* node = object.cast<NodeLifecycle*>();
    node->OnError(new_status);
    return;
  }

  const NodeInfo& node_info = response->node_info();

  NodeLifecycle* node = object.cast<NodeLifecycle*>();
  node->OnDidCreate(node_info);
}

void AddMasterProxy(py::module& m) {
  py::class_<PyMasterProxy>(m, "MasterProxy")
      .def_static("start", &PyMasterProxy::Start,
                  py::call_guard<py::gil_scoped_release>())
      .def_static("stop", &PyMasterProxy::Stop,
                  py::call_guard<py::gil_scoped_release>())
      .def_static("run", &PyMasterProxy::Run,
                  py::call_guard<py::gil_scoped_release>())
      .def_static("request_register_node", &PyMasterProxy::RequestRegisterNode)
      .def_static("post_task",
                  [](py::function callback) {
                    MasterProxy& master_proxy = MasterProxy::GetInstance();
                    master_proxy.PostTask(
                        FROM_HERE,
                        base::BindOnce(&PyClosure::Invoke,
                                       base::Owned(new PyClosure(callback))));
                  },
                  py::arg("callback"), py::call_guard<py::gil_scoped_release>())
      .def_static("post_delayed_task",
                  [](py::function callback, base::TimeDelta delay) {
                    MasterProxy& master_proxy = MasterProxy::GetInstance();
                    master_proxy.PostDelayedTask(
                        FROM_HERE,
                        base::BindOnce(&PyClosure::Invoke,
                                       base::Owned(new PyClosure(callback))),
                        delay);
                  },
                  py::arg("callback"), py::arg("delay"),
                  py::call_guard<py::gil_scoped_release>());
}

}  // namespace felicia