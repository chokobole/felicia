#if defined(HAS_ROS)

#include "felicia/python/rpc/ros_serialized_service_py.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/message/message_io_error.h"
#include "felicia/python/message/message_util.h"
#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {
namespace rpc {

PyRosSerializedServiceBridge::PyRosSerializedServiceBridge() = default;

PyRosSerializedServiceBridge::PyRosSerializedServiceBridge(py::object service)
    : service_(service) {
  PyRosSerializedService* py_service = service_.cast<PyRosSerializedService*>();
  py::object ros_service = py_service->ros_service();
  request_type_ = ros_service.attr("_request_class");
  response_type_ = ros_service.attr("_response_class");
  service_type_name_ =
      GetMessageTypeNameFromPyObject(ros_service, TopicInfo::ROS);
  service_md5sum_ = GetMessageMD5SumFromPyObject(ros_service, TopicInfo::ROS);
  request_type_name_ =
      GetMessageTypeNameFromPyObject(request_type_, TopicInfo::ROS);
  response_type_name_ =
      GetMessageTypeNameFromPyObject(response_type_, TopicInfo::ROS);
}

PyRosSerializedServiceBridge::~PyRosSerializedServiceBridge() = default;

void PyRosSerializedServiceBridge::Handle(const SerializedMessage* request,
                                          SerializedMessage* response,
                                          StatusOnceCallback callback) {
  PyRosSerializedService* py_service = service_.cast<PyRosSerializedService*>();
  py::gil_scoped_acquire acquire;
  py::object py_request = request_type_();
  py::object py_response = response_type_();
  Status s = Deserialize(request->serialized(), TopicInfo::ROS, &py_request);
  if (!s.ok()) {
    std::move(callback).Run(s);
    return;
  }
  internal::StatusOnceCallbackHolder* callback_holder =
      new internal::StatusOnceCallbackHolder(std::move(callback));
  auto py_callback = [callback_holder, response, py_response](Status s) {
    if (s.ok()) {
      std::string text;
      s = Serialize(py_response, TopicInfo::ROS, &text);
      if (!s.ok()) {
        py::gil_scoped_release release;
        callback_holder->Invoke(std::move(s));
        return;
      }
      response->set_serialized((std::move(text)));
    }
    py::gil_scoped_release release;
    callback_holder->Invoke(std::move(s));
  };
  py_service->Handle(py_request, py_response, py_callback);
}

void AddRosSerializedService(py::module& m) {
  py::class_<PyRosSerializedServiceInterface, PyRosSerializedService>(
      m, "RosService")
      .def(py::init<py::object>(), py::arg("ros_service"));
}

}  // namespace rpc
}  // namespace felicia

#endif  // defined(HAS_ROS)