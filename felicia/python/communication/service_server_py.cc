#include "felicia/python/communication/service_server_py.h"

#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {

PyServerBridge::PyServerBridge() = default;

PyServerBridge::PyServerBridge(py::object server) : server_(server) {}

ChannelDef PyServerBridge::channel_def() const {
  rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
  return py_server->channel_def();
}

void PyServerBridge::set_service_info(const ServiceInfo& service_info) {
  rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
  return py_server->set_service_info(service_info);
}

Status PyServerBridge::Start() {
  rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
  return py_server->Start();
}

Status PyServerBridge::Run() {
  rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
  return py_server->Run();
}

Status PyServerBridge::Shutdown() {
  rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
  return py_server->Shutdown();
}

std::string PyServerBridge::service_type() const {
  rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
  return py_server->service_type();
}

PyServiceServer::PyServiceServer(py::object server) {
  server_ = PyServerBridge(server);
}

void PyServiceServer::RequestRegister(const NodeInfo& node_info,
                                      const std::string& service,
                                      py::function py_callback) {
  StatusOnceCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }
  ServiceServer<EmptyService, PyServerBridge>::RequestRegister(
      node_info, service, std::move(callback));
}

void PyServiceServer::RequestUnregister(const NodeInfo& node_info,
                                        const std::string& service,
                                        py::function py_callback) {
  StatusOnceCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }
  ServiceServer<EmptyService, PyServerBridge>::RequestUnregister(
      node_info, service, std::move(callback));
}

void AddServiceServer(py::module& m) {
  py::class_<PyServiceServer>(m, "ServiceServer")
      .def(py::init<py::object>(), py::arg("server"))
      .def("is_registering", &PyServiceServer::IsRegistering)
      .def("is_registered", &PyServiceServer::IsRegistered)
      .def("is_unregistering", &PyServiceServer::IsUnregistering)
      .def("is_unregistered", &PyServiceServer::IsUnregistered)
      .def("request_register",
           [](PyServiceServer& self, const NodeInfo& node_info,
              const std::string& service) {
             self.RequestRegister(node_info, service, py::none());
           },
           py::arg("node_info"), py::arg("service"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_register",
           [](PyServiceServer& self, const NodeInfo& node_info,
              const std::string& service, py::function callback) {
             self.RequestRegister(node_info, service, callback);
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("callback").none(true),
           py::call_guard<py::gil_scoped_release>())
      .def("request_unregister",
           [](PyServiceServer& self, const NodeInfo& node_info,
              const std::string& service) {
             self.RequestUnregister(node_info, service, py::none());
           },
           py::arg("node_info"), py::arg("service"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_unregister",
           [](PyServiceServer& self, const NodeInfo& node_info,
              const std::string& service, py::function callback) {
             self.RequestUnregister(node_info, service, callback);
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("callback").none(true),
           py::call_guard<py::gil_scoped_release>());
}

}  // namespace felicia