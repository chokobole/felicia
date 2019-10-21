#include "felicia/python/communication/service_client_py.h"

#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {

using PyOnConnectCallback = PyCallback<void(ServiceInfo::Status)>;

PyClientBridge::PyClientBridge() = default;

PyClientBridge::PyClientBridge(py::object client) : client_(client) {}

void PyClientBridge::set_service_info(const ServiceInfo& service_info) {
  rpc::PyClient* py_client = client_.cast<rpc::PyClient*>();
  py_client->set_service_info(service_info);
}

void PyClientBridge::Connect(const IPEndPoint& ip_endpoint,
                             StatusOnceCallback callback) {
  rpc::PyClient* py_client = client_.cast<rpc::PyClient*>();
  internal::StatusOnceCallbackHolder* callback_holder =
      new internal::StatusOnceCallbackHolder(std::move(callback));
  auto py_callback = [callback_holder](const Status& s) {
    callback_holder->Invoke(s);
  };
  return py_client->Connect(ip_endpoint, py_callback);
}

Status PyClientBridge::Run() {
  rpc::PyClient* py_client = client_.cast<rpc::PyClient*>();
  return py_client->Run();
}

Status PyClientBridge::Shutdown() {
  rpc::PyClient* py_client = client_.cast<rpc::PyClient*>();
  return py_client->Shutdown();
}

PyServiceClient::PyServiceClient(py::object client) {
  client_ = PyClientBridge(client);
}

void PyServiceClient::RequestRegister(const NodeInfo& node_info,
                                      const std::string& service,
                                      py::function py_on_connect_callback,
                                      py::function py_callback) {
  OnConnectCallback on_connect_callback;
  StatusOnceCallback callback;
  if (!py_on_connect_callback.is_none()) {
    on_connect_callback = base::BindRepeating(
        &PyOnConnectCallback::Invoke,
        base::Owned(new PyOnConnectCallback(py_on_connect_callback)));
  }

  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }
  ServiceClient<PyClientBridge>::RequestRegister(
      node_info, service, on_connect_callback, std::move(callback));
}

void PyServiceClient::RequestUnregister(const NodeInfo& node_info,
                                        const std::string& service,
                                        py::function py_callback) {
  StatusOnceCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }
  ServiceClient<PyClientBridge>::RequestUnregister(node_info, service,
                                                   std::move(callback));
}

void AddServiceClient(py::module& m) {
  py::class_<PyServiceClient>(m, "ServiceClient")
      .def(py::init<py::object>(), py::arg("client"))
      .def("is_registering", &PyServiceClient::IsRegistering)
      .def("is_registered", &PyServiceClient::IsRegistered)
      .def("is_unregistering", &PyServiceClient::IsUnregistering)
      .def("is_unregistered", &PyServiceClient::IsUnregistered)
      .def("request_register",
           [](PyServiceClient& self, const NodeInfo& node_info,
              const std::string& service) {
             self.RequestRegister(node_info, service, py::none(), py::none());
           },
           py::arg("node_info"), py::arg("service"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_register",
           [](PyServiceClient& self, const NodeInfo& node_info,
              const std::string& service, py::function on_connect_callback) {
             self.RequestRegister(node_info, service, on_connect_callback,
                                  py::none());
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("on_connect_callback").none(true),
           py::call_guard<py::gil_scoped_release>())
      .def("request_register",
           [](PyServiceClient& self, const NodeInfo& node_info,
              const std::string& service, py::function on_connect_callback,
              py::function callback) {
             self.RequestRegister(node_info, service, on_connect_callback,
                                  callback);
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("on_connect_callback").none(true),
           py::arg("callback").none(true),
           py::call_guard<py::gil_scoped_release>())
      .def("request_unregister",
           [](PyServiceClient& self, const NodeInfo& node_info,
              const std::string& service) {
             self.RequestUnregister(node_info, service, py::none());
           },
           py::arg("node_info"), py::arg("service"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_unregister",
           [](PyServiceClient& self, const NodeInfo& node_info,
              const std::string& service, py::function callback) {
             self.RequestUnregister(node_info, service, callback);
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("callback").none(true),
           py::call_guard<py::gil_scoped_release>());
}

}  // namespace felicia