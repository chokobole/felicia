#include "felicia/python/communication_py.h"

#include "felicia/core/communication/serialized_message_publisher.h"
#include "felicia/core/communication/serialized_message_subscriber.h"
#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(felicia::ChannelDef, ChannelDef,
                           felicia.core.protobuf.channel_pb2)
SUPPORT_PROTOBUF_TYPE_CAST(felicia::NodeInfo, NodeInfo,
                           felicia.core.protobuf.master_data_pb2)
SUPPORT_PROTOBUF_ENUM_TYPE_CAST(felicia::ChannelDef::Type, ChannelDef.Type)
SUPPORT_PROTOBUF_ENUM_TYPE_CAST(felicia::TopicInfo::ImplType,
                                TopicInfo.ImplType)

namespace felicia {

namespace {

class PyMessageCallback {
 public:
  PyMessageCallback(py::object message_prototype, TopicInfo::ImplType impl_type,
                    const py::function& func)
      : message_prototype_(message_prototype),
        impl_type_(impl_type),
        func_(func) {}

  void Invoke(SerializedMessage&& message) {
    std::string text;
    if (!std::move(message).SerializeToString(&text)) {
      return;
    }

    py::gil_scoped_acquire acquire;
    py::object object = message_prototype_();
    switch (impl_type_) {
      case TopicInfo::PROTOBUF: {
        object.attr("ParseFromString")(py::bytes(text));
        break;
      }
      case TopicInfo::ROS: {
        object.attr("deserialize")(py::bytes(text));
        break;
      }
      case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
      case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
    object.inc_ref();
    func_(std::move(object));
  }

 private:
  py::object message_prototype_;
  TopicInfo::ImplType impl_type_;
  py::function func_;
};

}  // namespace

using PySendMessageCallback = PyCallback<void(ChannelDef::Type, const Status&)>;

namespace {

bool SerializeToString(SerializedMessagePublisher& self, py::object message,
                       std::string* text) {
  TopicInfo::ImplType impl_type = self.GetMessageImplType();
  switch (impl_type) {
    case TopicInfo::PROTOBUF: {
      *text = pybind11::str(message.attr("SerializeToString")());
      break;
    }
    case TopicInfo::ROS: {
      py::object buffer = pybind11::module::import("io").attr("BytesIO")();
      message.attr("serialize")(buffer);
      *text = pybind11::str(buffer.attr("getvalue")());
      break;
    }
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
      break;
  }

  if (PyErr_Occurred()) {
    LOG(ERROR) << errors::InvalidArgument(
        MessageIOErrorToString(MessageIOError::ERR_FAILED_TO_SERIALIZE));
    return false;
  }
  return true;
}

}  // namespace

void AddCommunication(py::module& m) {
  py::module communication = m.def_submodule("communication");

  py::class_<communication::Settings>(communication, "Settings")
      .def(py::init<>())
      .def_readwrite("period", &communication::Settings::period)
      .def_readwrite("buffer_size", &communication::Settings::buffer_size)
      .def_readwrite("is_dynamic_buffer",
                     &communication::Settings::is_dynamic_buffer)
      .def_readwrite("queue_size", &communication::Settings::queue_size)
      .def_readwrite("channel_settings",
                     &communication::Settings::channel_settings);

  py::class_<SerializedMessagePublisher>(communication, "Publisher")
      .def(py::init<>())
      .def("is_registering", &SerializedMessagePublisher::IsRegistering)
      .def("is_registered", &SerializedMessagePublisher::IsRegistered)
      .def("is_unregistering", &SerializedMessagePublisher::IsUnregistering)
      .def("is_unregistered", &SerializedMessagePublisher::IsUnregistered)
      .def("set_message_impl_type",
           &SerializedMessagePublisher::SetMessageImplType)
      .def("request_publish",
           [](SerializedMessagePublisher& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              const std::string message_type,
              const communication::Settings& settings) {
             self.SetMessageTypeName(message_type);

             self.RequestPublish(node_info, topic, channel_types, settings);
           },
           py::arg("node_info"), py::arg("topic"), py::arg("channel_types"),
           py::arg("message_type"), py::arg("settings"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_publish",
           [](SerializedMessagePublisher& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              const std::string message_type,
              const communication::Settings& settings, py::function callback) {
             self.SetMessageTypeName(message_type);

             self.RequestPublish(
                 node_info, topic, channel_types, settings,
                 base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(callback))));
           },
           py::arg("node_info"), py::arg("topic"), py::arg("channel_types"),
           py::arg("message_type"), py::arg("settings"), py::arg("callback"),
           py::call_guard<py::gil_scoped_release>())
      .def("publish",
           [](SerializedMessagePublisher& self, py::object message) {
             std::string text;
             if (!SerializeToString(self, message, &text)) return;

             py::gil_scoped_release release;
             self.PublishFromSerialized(std::move(text));
           },
           py::arg("message"))
      .def("publish",
           [](SerializedMessagePublisher& self, py::object message,
              py::function callback) {
             std::string text;
             if (!SerializeToString(self, message, &text)) return;

             py::gil_scoped_release release;
             self.PublishFromSerialized(
                 std::move(text),
                 base::BindRepeating(
                     &PySendMessageCallback::Invoke,
                     base::Owned(new PySendMessageCallback(callback))));
           },
           py::arg("message"), py::arg("callback"))
      .def("request_unpublish",
           [](SerializedMessagePublisher& self, const NodeInfo& node_info,
              const std::string& topic) {
             self.RequestUnpublish(node_info, topic);
           },
           py::arg("node_info"), py::arg("topic"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_unpublish",
           [](SerializedMessagePublisher& self, const NodeInfo& node_info,
              const std::string& topic, py::function callback) {
             self.RequestUnpublish(
                 node_info, topic,
                 base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(callback))));
           },
           py::arg("node_info"), py::arg("topic"), py::arg("callback"),
           py::call_guard<py::gil_scoped_release>());

  py::class_<SerializedMessageSubscriber>(communication, "Subscriber")
      .def(py::init<>())
      .def("is_registering", &SerializedMessageSubscriber::IsRegistering)
      .def("is_registered", &SerializedMessageSubscriber::IsRegistered)
      .def("is_unregistering", &SerializedMessageSubscriber::IsUnregistering)
      .def("is_unregistered", &SerializedMessageSubscriber::IsUnregistered)
      .def("is_started", &SerializedMessageSubscriber::IsStarted)
      .def("is_stopped", &SerializedMessageSubscriber::IsStopped)
      .def("set_message_impl_type",
           &SerializedMessageSubscriber::SetMessageImplType)
      .def("request_subscribe",
           [](SerializedMessageSubscriber& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              py::object mesage_prototype,
              const communication::Settings& settings,
              py::function on_message_callback) {
             self.RequestSubscribe(
                 node_info, topic, channel_types, settings,
                 base::BindRepeating(
                     &PyMessageCallback::Invoke,
                     base::Owned(new PyMessageCallback(
                         mesage_prototype, self.GetMessageImplType(),
                         on_message_callback))));
           },
           py::arg("node_info"), py::arg("topic"), py::arg("channel_types"),
           py::arg("message_prototype"), py::arg("settings"),
           py::arg("on_message_callback"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_subscribe",
           [](SerializedMessageSubscriber& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              py::object mesage_prototype,
              const communication::Settings& settings,
              py::function on_message_callback,
              py::function on_message_error_callback) {
             self.RequestSubscribe(
                 node_info, topic, channel_types, settings,
                 base::BindRepeating(
                     &PyMessageCallback::Invoke,
                     base::Owned(new PyMessageCallback(
                         mesage_prototype, self.GetMessageImplType(),
                         on_message_callback))),
                 base::BindRepeating(&PyStatusCallback::Invoke,
                                     base::Owned(new PyStatusCallback(
                                         on_message_error_callback))));
           },
           py::arg("node_info"), py::arg("topic"), py::arg("channel_types"),
           py::arg("message_prototype"), py::arg("settings"),
           py::arg("on_message_callback"), py::arg("on_message_error_callback"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_subscribe",
           [](SerializedMessageSubscriber& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              py::object mesage_prototype,
              const communication::Settings& settings,
              py::function on_message_callback,
              py::function on_message_error_callback, py::function callback) {
             self.RequestSubscribe(
                 node_info, topic, channel_types, settings,
                 base::BindRepeating(
                     &PyMessageCallback::Invoke,
                     base::Owned(new PyMessageCallback(
                         mesage_prototype, self.GetMessageImplType(),
                         on_message_callback))),
                 base::BindRepeating(&PyStatusCallback::Invoke,
                                     base::Owned(new PyStatusCallback(
                                         on_message_error_callback))),
                 base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(callback))));
           },
           py::arg("node_info"), py::arg("topic"), py::arg("channel_types"),
           py::arg("message_prototype"), py::arg("settings"),
           py::arg("on_message_callback"), py::arg("on_message_error_callback"),
           py::arg("callback"), py::call_guard<py::gil_scoped_release>())
      .def("request_unsubscribe",
           [](SerializedMessageSubscriber& self, const NodeInfo& node_info,
              const std::string& topic) {
             self.RequestUnsubscribe(node_info, topic);
           },
           py::arg("node_info"), py::arg("topic"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_unsubscribe",
           [](SerializedMessageSubscriber& self, const NodeInfo& node_info,
              const std::string& topic, py::function callback) {
             self.RequestUnsubscribe(
                 node_info, topic,
                 base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(callback))));
           },
           py::arg("node_info"), py::arg("topic"), py::arg("callback"),
           py::call_guard<py::gil_scoped_release>());
}

}  // namespace felicia