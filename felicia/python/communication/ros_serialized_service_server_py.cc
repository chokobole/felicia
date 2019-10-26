#if defined(HAS_ROS)

#include "felicia/python/communication/ros_serialized_service_server_py.h"

#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {

PyRosSerializedServiceServer::PyRosSerializedServiceServer(py::object service) {
  server_ = rpc::PyRosSerializedServer(service);
}

void PyRosSerializedServiceServer::RequestRegister(const NodeInfo& node_info,
                                                   const std::string& service,
                                                   py::function py_callback) {
  StatusOnceCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }

  py::gil_scoped_release release;
  ServiceServer<EmptyService, rpc::PyRosSerializedServer>::RequestRegister(
      node_info, service, std::move(callback));
}

void PyRosSerializedServiceServer::RequestUnregister(const NodeInfo& node_info,
                                                     const std::string& service,
                                                     py::function py_callback) {
  StatusOnceCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }

  py::gil_scoped_release release;
  ServiceServer<EmptyService, rpc::PyRosSerializedServer>::RequestUnregister(
      node_info, service, std::move(callback));
}

void AddRosSerializedServiceServer(py::module& m) {
  py::class_<PyRosSerializedServiceServer>(m, "RosServiceServer")
      .def(py::init<py::object>(), py::arg("service"))
      .def("is_registering", &PyRosSerializedServiceServer::IsRegistering)
      .def("is_registered", &PyRosSerializedServiceServer::IsRegistered)
      .def("is_unregistering", &PyRosSerializedServiceServer::IsUnregistering)
      .def("is_unregistered", &PyRosSerializedServiceServer::IsUnregistered)
      .def("request_register",
           [](PyRosSerializedServiceServer& self, const NodeInfo& node_info,
              const std::string& service) {
             self.RequestRegister(node_info, service, py::none());
           },
           py::arg("node_info"), py::arg("service"))
      .def("request_register",
           [](PyRosSerializedServiceServer& self, const NodeInfo& node_info,
              const std::string& service, py::function callback) {
             self.RequestRegister(node_info, service, callback);
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("callback").none(true))
      .def("request_unregister",
           [](PyRosSerializedServiceServer& self, const NodeInfo& node_info,
              const std::string& service) {
             self.RequestUnregister(node_info, service, py::none());
           },
           py::arg("node_info"), py::arg("service"))
      .def("request_unregister",
           [](PyRosSerializedServiceServer& self, const NodeInfo& node_info,
              const std::string& service, py::function callback) {
             self.RequestUnregister(node_info, service, callback);
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("callback").none(true));
}

}  // namespace felicia

#endif  // defined(HAS_ROS)