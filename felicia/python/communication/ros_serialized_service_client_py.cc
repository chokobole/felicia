#if defined(HAS_ROS)

#include "felicia/python/communication/ros_serialized_service_client_py.h"

#include "felicia/python/message/message_util.h"
#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {

namespace {

class PyCallCallback {
 public:
  PyCallCallback(py::object response, py::function callback)
      : response_(response), callback_(callback) {}

  void Invoke(const SerializedMessage* request,
              const SerializedMessage* response, Status s) {
    py::gil_scoped_acquire acquire;
    if (s.ok()) {
      s = Deserialize(response->serialized(), TopicInfo::ROS, &response_);
    }
    callback_(std::move(s));
  }

 private:
  py::object response_;
  py::function callback_;
};

}  // namespace

using PyOnConnectCallback = PyCallback<void(ServiceInfo::Status)>;

PyRosSerializedServiceClient::PyRosSerializedServiceClient(
    py::object ros_service) {
  client_ = rpc::PyRosSerializedClient(ros_service);
}

void PyRosSerializedServiceClient::RequestRegister(
    const NodeInfo& node_info, const std::string& service,
    py::function py_on_connect_callback, py::function py_callback) {
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
  ServiceClient<rpc::PyRosSerializedClient>::RequestRegister(
      node_info, service, on_connect_callback, std::move(callback));
}

void PyRosSerializedServiceClient::RequestUnregister(const NodeInfo& node_info,
                                                     const std::string& service,
                                                     py::function py_callback) {
  StatusOnceCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }
  ServiceClient<rpc::PyRosSerializedClient>::RequestUnregister(
      node_info, service, std::move(callback));
}

void PyRosSerializedServiceClient::Call(py::object py_request,
                                        py::object py_response,
                                        py::function py_callback) {
  std::string text;
  Status s = Serialize(py_request, TopicInfo::ROS, &text);
  if (!s.ok()) {
    py_callback(s);
    return;
  }
  py::gil_scoped_release release;
  SerializedMessage* request = new SerializedMessage();
  SerializedMessage* response = new SerializedMessage();
  request->set_serialized(std::move(text));
  py_callback.inc_ref();

  client_.Call(
      request, response,
      base::BindOnce(&PyCallCallback::Invoke,
                     base::Owned(new PyCallCallback(py_response, py_callback)),
                     base::Owned(request), base::Owned(response)));
}

void AddRosSerializedServiceClient(py::module& m) {
  py::class_<PyRosSerializedServiceClient>(m, "RosServiceClient")
      .def(py::init<py::object>(), py::arg("ros_service"))
      .def("is_registering", &PyRosSerializedServiceClient::IsRegistering)
      .def("is_registered", &PyRosSerializedServiceClient::IsRegistered)
      .def("is_unregistering", &PyRosSerializedServiceClient::IsUnregistering)
      .def("is_unregistered", &PyRosSerializedServiceClient::IsUnregistered)
      .def("request_register",
           [](PyRosSerializedServiceClient& self, const NodeInfo& node_info,
              const std::string& service) {
             self.RequestRegister(node_info, service, py::none(), py::none());
           },
           py::arg("node_info"), py::arg("service"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_register",
           [](PyRosSerializedServiceClient& self, const NodeInfo& node_info,
              const std::string& service, py::function on_connect_callback) {
             self.RequestRegister(node_info, service, on_connect_callback,
                                  py::none());
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("on_connect_callback").none(true),
           py::call_guard<py::gil_scoped_release>())
      .def("request_register",
           [](PyRosSerializedServiceClient& self, const NodeInfo& node_info,
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
           [](PyRosSerializedServiceClient& self, const NodeInfo& node_info,
              const std::string& service) {
             self.RequestUnregister(node_info, service, py::none());
           },
           py::arg("node_info"), py::arg("service"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_unregister",
           [](PyRosSerializedServiceClient& self, const NodeInfo& node_info,
              const std::string& service, py::function callback) {
             self.RequestUnregister(node_info, service, callback);
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("callback").none(true),
           py::call_guard<py::gil_scoped_release>())
      .def("call",
           [](PyRosSerializedServiceClient& self, py::object request,
              py::object response, py::function callback) {
             self.Call(request, response, callback);
           },
           py::arg("node_info"), py::arg("service"),
           py::arg("callback").none(true));
}

}  // namespace felicia

#endif  // defined(HAS_ROS)