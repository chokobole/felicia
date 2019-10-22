#include "felicia/python/communication/serialized_message_publisher_py.h"

#include "felicia/python/message/message_util.h"
#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {

using PySendMessageCallback = PyCallback<void(ChannelDef::Type, const Status&)>;

PySerializedMessagePublisher::PySerializedMessagePublisher(
    py::object message_type, TopicInfo::ImplType impl_type) {
#if defined(HAS_ROS)
  message_md5_sum_ = GetMessageMD5SumFromPyObject(message_type, impl_type);
  message_definition_ =
      GetMessageDefinitionFromPyObject(message_type, impl_type);
#endif  // defined(HAS_ROS)
  message_type_name_ = GetMessageTypeNameFromPyObject(message_type, impl_type);
  impl_type_ = impl_type;
}

void PySerializedMessagePublisher::RequestPublish(
    const NodeInfo& node_info, const std::string& topic, int channel_types,
    const communication::Settings& settings, py::function py_callback) {
  StatusOnceCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }
  SerializedMessagePublisher::RequestPublish(node_info, topic, channel_types,
                                             settings, std::move(callback));
}

void PySerializedMessagePublisher::RequestUnpublish(const NodeInfo& node_info,
                                                    const std::string& topic,
                                                    py::function py_callback) {
  StatusOnceCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindOnce(&PyStatusCallback::Invoke,
                              base::Owned(new PyStatusCallback(py_callback)));
  }
  SerializedMessagePublisher::RequestUnpublish(node_info, topic,
                                               std::move(callback));
}

void PySerializedMessagePublisher::PublishFromSerialized(
    py::object message, py::function py_callback) {
  std::string text;
  Status s = Serialize(message, impl_type_, &text);
  if (!s.ok()) {
    py_callback(ChannelDef::CHANNEL_TYPE_NONE, s);
    return;
  }

  py::gil_scoped_release release;
  SendMessageCallback callback;
  if (!py_callback.is_none()) {
    callback = base::BindRepeating(
        &PySendMessageCallback::Invoke,
        base::Owned(new PySendMessageCallback(py_callback)));
  }
  SerializedMessagePublisher::PublishFromSerialized(std::move(text), callback);
}

void AddSerializedMessagePublisher(py::module& m) {
  py::class_<PySerializedMessagePublisher>(m, "Publisher")
      .def(py::init<py::object, TopicInfo::ImplType>(), py::arg("message_type"),
           py::arg("impl_type") = TopicInfo::PROTOBUF)
      .def("is_registering", &PySerializedMessagePublisher::IsRegistering)
      .def("is_registered", &PySerializedMessagePublisher::IsRegistered)
      .def("is_unregistering", &PySerializedMessagePublisher::IsUnregistering)
      .def("is_unregistered", &PySerializedMessagePublisher::IsUnregistered)
      .def("request_publish",
           [](PySerializedMessagePublisher& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              const communication::Settings& settings) {
             self.RequestPublish(node_info, topic, channel_types, settings,
                                 py::none());
           },
           py::arg("node_info"), py::arg("topic"), py::arg("channel_types"),
           py::arg("settings"), py::call_guard<py::gil_scoped_release>())
      .def("request_publish",
           [](PySerializedMessagePublisher& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              const communication::Settings& settings, py::function callback) {
             self.RequestPublish(node_info, topic, channel_types, settings,
                                 callback);
           },
           py::arg("node_info"), py::arg("topic"), py::arg("channel_types"),
           py::arg("settings"), py::arg("callback").none(true),
           py::call_guard<py::gil_scoped_release>())
      .def("publish",
           [](PySerializedMessagePublisher& self, py::object message) {
             self.PublishFromSerialized(message, py::none());
           },
           py::arg("message"))
      .def("publish",
           [](PySerializedMessagePublisher& self, py::object message,
              py::function callback) {
             self.PublishFromSerialized(message, callback);
           },
           py::arg("message"), py::arg("callback").none(true))
      .def("request_unpublish",
           [](PySerializedMessagePublisher& self, const NodeInfo& node_info,
              const std::string& topic) {
             self.RequestUnpublish(node_info, topic, py::none());
           },
           py::arg("node_info"), py::arg("topic"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_unpublish",
           [](PySerializedMessagePublisher& self, const NodeInfo& node_info,
              const std::string& topic, py::function callback) {
             self.RequestUnpublish(node_info, topic, callback);
           },
           py::arg("node_info"), py::arg("topic"),
           py::arg("callback").none(true),
           py::call_guard<py::gil_scoped_release>());
}

}  // namespace felicia