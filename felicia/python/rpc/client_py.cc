#include "felicia/python/rpc/client_py.h"

#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"
#include "felicia/python/type_conversion/util.h"

namespace felicia {
namespace rpc {

PyClientBridge::PyClientBridge() = default;

PyClientBridge::PyClientBridge(py::object client) : client_(client) {}

void PyClientBridge::set_service_info(const ServiceInfo& service_info) {
  PyClient* py_client = client_.cast<PyClient*>();
  py_client->set_service_info(service_info);
}

void PyClientBridge::Connect(const IPEndPoint& ip_endpoint,
                             StatusOnceCallback callback) {
  PyClient* py_client = client_.cast<PyClient*>();
  internal::StatusOnceCallbackHolder* callback_holder =
      new internal::StatusOnceCallbackHolder(std::move(callback));
  auto py_callback = [callback_holder](Status s) {
    py::gil_scoped_release release;
    callback_holder->Invoke(std::move(s));
  };
  py_client->Connect(ip_endpoint, py_callback);
}

Status PyClientBridge::Run() {
  PyClient* py_client = client_.cast<PyClient*>();
  return py_client->Run();
}

Status PyClientBridge::Shutdown() {
  PyClient* py_client = client_.cast<PyClient*>();
  return py_client->Shutdown();
}

void AddClient(py::module& m) {
  py::class_<PyClientInterface, PyClient>(m, "_Client")
      .def(py::init<>())
      .def("service_info", &PyClientInterface::service_info);
}

}  // namespace rpc
}  // namespace felicia