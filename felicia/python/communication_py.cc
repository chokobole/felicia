#include "felicia/python/communication_py.h"

#include "felicia/core/communication/dynamic_publisher.h"
#include "felicia/core/communication/dynamic_subscriber.h"
#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(::felicia::ChannelDef, ChannelDef,
                           felicia.core.protobuf.channel_pb2)
SUPPORT_PROTOBUF_TYPE_CAST(::felicia::NodeInfo, NodeInfo,
                           felicia.core.protobuf.master_data_pb2)
SUPPORT_PROTOBUF_ENUM_TYPE_CAST(::felicia::ChannelDef::Type, ChannelDef.Type)

namespace felicia {

namespace {

class PyMessageCallback {
 public:
  PyMessageCallback(py::object message_prototype, py::function func)
      : message_prototype_(message_prototype), func_(func) {}

  void Invoke(DynamicProtobufMessage&& message) {
    std::string text;
    if (!message.SerializeToString(&text)) {
      return;
    }

    py::object object = message_prototype_();
    object.attr("ParseFromString")(py::bytes(text));
    object.inc_ref();
    func_(std::move(object));
  }

 private:
  py::object message_prototype_;
  py::function func_;
};

}  // namespace

using PySendMessageCallback = PyCallback<void(ChannelDef::Type, const Status&)>;

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

  py::class_<DynamicPublisher>(communication, "Publisher")
      .def(py::init<>())
      .def("is_registering", &DynamicPublisher::IsRegistering)
      .def("is_registered", &DynamicPublisher::IsRegistered)
      .def("is_unregistering", &DynamicPublisher::IsUnregistering)
      .def("is_unregistered", &DynamicPublisher::IsUnregistered)
      .def("request_publish",
           [](DynamicPublisher& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              const std::string message_type,
              const communication::Settings& settings, py::function callback) {
             self.ResetMessage(message_type);

             return self.RequestPublish(
                 node_info, topic, channel_types, settings,
                 ::base::BindOnce(
                     &PyStatusCallback::Invoke,
                     ::base::Owned(new PyStatusCallback(callback))));
           })
      .def("publish",
           [](DynamicPublisher& self, py::object message,
              py::function callback) {
             std::string text =
                 pybind11::str(message.attr("SerializeToString")());
             if (PyErr_Occurred()) {
               callback(errors::InvalidArgument(MessageIoErrorToString(
                   MessageIoError::ERR_FAILED_TO_SERIALIZE)));
               return;
             }

             return self.PublishFromSerialized(
                 text, ::base::BindRepeating(
                           &PySendMessageCallback::Invoke,
                           ::base::Owned(new PySendMessageCallback(callback))));
           })
      .def("request_unpublish", [](DynamicPublisher& self,
                                   const NodeInfo& node_info,
                                   const std::string& topic,
                                   py::function callback) {
        return self.RequestUnpublish(
            node_info, topic,
            ::base::BindOnce(&PyStatusCallback::Invoke,
                             ::base::Owned(new PyStatusCallback(callback))));
      });

  py::class_<DynamicSubscriber>(communication, "Subscriber")
      .def(py::init<>())
      .def("is_registering", &DynamicSubscriber::IsRegistering)
      .def("is_registered", &DynamicSubscriber::IsRegistered)
      .def("is_unregistering", &DynamicSubscriber::IsUnregistering)
      .def("is_unregistered", &DynamicSubscriber::IsUnregistered)
      .def("is_started", &DynamicSubscriber::IsStarted)
      .def("is_stopped", &DynamicSubscriber::IsStopped)
      .def("request_subscribe",
           [](DynamicSubscriber& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              py::object mesage_prototype, py::function on_message_callback,
              py::function on_error_callback,
              const communication::Settings& settings, py::function callback) {
             return self.RequestSubscribe(
                 node_info, topic, channel_types,
                 ::base::BindRepeating(
                     &PyMessageCallback::Invoke,
                     ::base::Owned(new PyMessageCallback(mesage_prototype,
                                                         on_message_callback))),
                 ::base::BindRepeating(
                     &PyStatusCallback::Invoke,
                     ::base::Owned(new PyStatusCallback(on_error_callback))),
                 settings,
                 ::base::BindOnce(
                     &PyStatusCallback::Invoke,
                     ::base::Owned(new PyStatusCallback(callback))));
           })
      .def("request_unsubscribe", [](DynamicSubscriber& self,
                                     const NodeInfo& node_info,
                                     const std::string& topic,
                                     py::function callback) {
        return self.RequestUnsubscribe(
            node_info, topic,
            ::base::BindOnce(&PyStatusCallback::Invoke,
                             ::base::Owned(new PyStatusCallback(callback))));
      });
}

}  // namespace felicia