#include "felicia/python/py_master_proxy.h"

#include <csignal>

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/master/master_proxy.h"

namespace felicia {

namespace {

void Shutdown(int signal) {
  MasterProxy& master_proxy = MasterProxy::GetInstance();
  master_proxy.Stop();
}

}  // namespace

// static
Status PyMasterProxy::Start() { return MasterProxy::GetInstance().Start(); }

// static
Status PyMasterProxy::Stop() { return MasterProxy::GetInstance().Stop(); }

// static
void PyMasterProxy::Run() {
  // To handle general case when POSIX ask the process to quit.
  std::signal(SIGTERM, &::felicia::Shutdown);
  // To handle Ctrl + C.
  std::signal(SIGINT, &::felicia::Shutdown);
  // To handle when the terminal is closed.
  std::signal(SIGHUP, &::felicia::Shutdown);

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
  NodeLifecycle* node = object.cast<NodeLifecycle*>();

  node->OnInit();
  master_proxy.RegisterNodeAsync(
      request, response,
      ::base::BindOnce(&PyMasterProxy::OnRegisterNodeAsync, object, request,
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
        ::base::BindOnce(&PyMasterProxy::OnRegisterNodeAsync, object,
                         ::base::Owned(request), ::base::Owned(response), s));
    return;
  }

  if (!s.ok()) {
    Status new_status(s.error_code(),
                      ::base::StringPrintf("Failed to register node : %s",
                                           s.error_message().c_str()));
    NodeLifecycle* node = object.cast<NodeLifecycle*>();
    node->OnError(new_status);
    return;
  }

  std::cout << ::base::PlatformThread::GetName() << std::endl;

  const NodeInfo& node_info = response->node_info();

  // py::object object = py::cast(node.get());
  NodeLifecycle* node = object.cast<NodeLifecycle*>();
  // object.attr("OnDidCreate")(node_info);
  node->OnDidCreate(node_info);
  // node->OnDidCreate(node_info);
  // master_proxy.nodes_.push_back(std::move(node));
}

}  // namespace felicia