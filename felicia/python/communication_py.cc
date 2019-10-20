#include "felicia/python/communication_py.h"

#include "felicia/core/communication/serialized_message_publisher.h"
#include "felicia/core/communication/serialized_message_subscriber.h"
#include "felicia/core/communication/service_client.h"
#include "felicia/core/communication/service_server.h"
#include "felicia/python/rpc_py.h"
#include "felicia/python/type_conversion/callback.h"
#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(felicia::ChannelDef, ChannelDef,
                           felicia.core.protobuf.channel_pb2)
SUPPORT_PROTOBUF_TYPE_CAST(felicia::NodeInfo, NodeInfo,
                           felicia.core.protobuf.master_data_pb2)
SUPPORT_PROTOBUF_ENUM_TYPE_CAST(felicia::ChannelDef::Type, ChannelDef.Type)
SUPPORT_PROTOBUF_ENUM_TYPE_CAST(felicia::TopicInfo::ImplType,
                                TopicInfo.ImplType)
SUPPORT_PROTOBUF_ENUM_TYPE_CAST(felicia::ServiceInfo::Status,
                                ServiceInfo.Status)

namespace felicia {

namespace {

class PyMessageCallback {
 public:
  PyMessageCallback(py::object message_type, TopicInfo::ImplType impl_type,
                    py::function callback)
      : message_type_(message_type),
        impl_type_(impl_type),
        callback_(callback) {}

  void Invoke(SerializedMessage&& message) {
    std::string text;
    if (!std::move(message).SerializeToString(&text)) {
      return;
    }

    py::gil_scoped_acquire acquire;
    py::object py_message = message_type_();
    switch (impl_type_) {
      case TopicInfo::PROTOBUF: {
        py_message.attr("ParseFromString")(py::bytes(text));
        break;
      }
      case TopicInfo::ROS: {
        py_message.attr("deserialize")(py::bytes(text));
        break;
      }
      case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
      case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
    callback_(std::move(py_message));
  }

 private:
  py::object message_type_;
  TopicInfo::ImplType impl_type_;
  py::function callback_;
};

using PySendMessageCallback = PyCallback<void(ChannelDef::Type, const Status&)>;
using PyOnConnectCallback = PyCallback<void(ServiceInfo::Status)>;

std::string GetMessageTypeNameFromPyObject(const py::object& message_type,
                                           TopicInfo::ImplType impl_type) {
  std::string message_type_name;
  switch (impl_type) {
    case TopicInfo::PROTOBUF: {
      message_type_name =
          py::str(message_type.attr("DESCRIPTOR").attr("full_name"));
      break;
    }
    case TopicInfo::ROS: {
      message_type_name = py::str(message_type.attr("_type"));
      break;
    }
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
      break;
  }
  return message_type_name;
}

std::string GetMessageMD5SumFromPyObject(const py::object& message_type,
                                         TopicInfo::ImplType impl_type) {
  std::string md5sum;
  switch (impl_type) {
    case TopicInfo::PROTOBUF:
      break;
    case TopicInfo::ROS: {
      md5sum = py::str(message_type.attr("_md5sum"));
      break;
    }
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
      break;
  }
  return md5sum;
}

std::string GetMessageDefinitionFromPyObject(const py::object& message_type,
                                             TopicInfo::ImplType impl_type) {
  std::string definition;
  switch (impl_type) {
    case TopicInfo::PROTOBUF:
      break;
    case TopicInfo::ROS: {
      definition = py::str(message_type.attr("_full_text"));
      break;
    }
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
    case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
      break;
  }
  return definition;
}

class PySerializedMessagePublisher : public SerializedMessagePublisher {
 public:
  explicit PySerializedMessagePublisher(
      py::object message_type,
      TopicInfo::ImplType impl_type = TopicInfo::PROTOBUF) {
#if defined(HAS_ROS)
    message_md5_sum_ = GetMessageMD5SumFromPyObject(message_type, impl_type);
    message_definition_ =
        GetMessageDefinitionFromPyObject(message_type, impl_type);
#endif  // defined(HAS_ROS)
    message_type_name_ =
        GetMessageTypeNameFromPyObject(message_type, impl_type);
    impl_type_ = impl_type;
  }

  void RequestPublish(const NodeInfo& node_info, const std::string& topic,
                      int channel_types,
                      const communication::Settings& settings,
                      py::function py_callback = py::none()) {
    StatusOnceCallback callback;
    if (!py_callback.is_none()) {
      callback = base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(py_callback)));
    }
    SerializedMessagePublisher::RequestPublish(node_info, topic, channel_types,
                                               settings, std::move(callback));
  }

  void RequestUnpublish(const NodeInfo& node_info, const std::string& topic,
                        py::function py_callback = py::none()) {
    StatusOnceCallback callback;
    if (!py_callback.is_none()) {
      callback = base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(py_callback)));
    }
    SerializedMessagePublisher::RequestUnpublish(node_info, topic,
                                                 std::move(callback));
  }

  void PublishFromSerialized(py::object message,
                             py::function py_callback = py::none()) {
    std::string text;
    MessageIOError err = SerializeToString(message, &text);
    if (err != MessageIOError::OK) return;

    py::gil_scoped_release release;
    SendMessageCallback callback;
    if (!py_callback.is_none()) {
      callback = base::BindRepeating(
          &PySendMessageCallback::Invoke,
          base::Owned(new PySendMessageCallback(py_callback)));
    }
    SerializedMessagePublisher::PublishFromSerialized(std::move(text),
                                                      callback);
  }

  MessageIOError SerializeToString(py::object message, std::string* text) {
    switch (impl_type_) {
      case TopicInfo::PROTOBUF: {
        *text = py::str(message.attr("SerializeToString")());
        break;
      }
      case TopicInfo::ROS: {
        py::object buffer = py::module::import("io").attr("BytesIO")();
        message.attr("serialize")(buffer);
        *text = py::str(buffer.attr("getvalue")());
        break;
      }
      case TopicInfo_ImplType_TopicInfo_ImplType_INT_MIN_SENTINEL_DO_NOT_USE_:
      case TopicInfo_ImplType_TopicInfo_ImplType_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }

    if (PyErr_Occurred()) {
      return MessageIOError::ERR_FAILED_TO_SERIALIZE;
    }
    return MessageIOError::OK;
  }
};

class PySerializedMessageSubscriber : public SerializedMessageSubscriber {
 public:
  explicit PySerializedMessageSubscriber(
      py::object message_type,
      TopicInfo::ImplType impl_type = TopicInfo::PROTOBUF)
      : message_type_(message_type) {
#if defined(HAS_ROS)
    message_md5_sum_ = GetMessageMD5SumFromPyObject(message_type, impl_type);
#endif  // defined(HAS_ROS)
    message_type_name_ =
        GetMessageTypeNameFromPyObject(message_type, impl_type);
    impl_type_ = impl_type;
  }

  void RequestSubscribe(const NodeInfo& node_info, const std::string& topic,
                        int channel_types,
                        const communication::Settings& settings,
                        py::function py_on_message_callback,
                        py::function py_on_message_error_callback = py::none(),
                        py::function py_callback = py::none()) {
    OnMessageCallback on_message_callback;
    StatusCallback on_message_error_callback;
    StatusOnceCallback callback;

    if (!py_on_message_callback.is_none()) {
      on_message_callback = base::BindRepeating(
          &PyMessageCallback::Invoke,
          base::Owned(new PyMessageCallback(message_type_, impl_type_,
                                            py_on_message_callback)));
    }

    if (!py_on_message_error_callback.is_none()) {
      on_message_error_callback = base::BindRepeating(
          &PyStatusCallback::Invoke,
          base::Owned(new PyStatusCallback(py_on_message_error_callback)));
    }

    if (!py_callback.is_none()) {
      callback = base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(py_callback)));
    }
    SerializedMessageSubscriber::RequestSubscribe(
        node_info, topic, channel_types, settings, on_message_callback,
        on_message_error_callback, std::move(callback));
  }

  void RequestUnsubscribe(const NodeInfo& node_info, const std::string& topic,
                          py::function py_callback = py::none()) {
    StatusOnceCallback callback;
    if (!py_callback.is_none()) {
      callback = base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(py_callback)));
    }
    SerializedMessageSubscriber::RequestUnsubscribe(node_info, topic,
                                                    std::move(callback));
  }

 private:
  py::object message_type_;
};

class PyClientBridge {
 public:
  PyClientBridge() = default;
  explicit PyClientBridge(py::object client) : client_(client) {}

  void set_service_info(const ServiceInfo& service_info) {
    rpc::PyClient* py_client = client_.cast<rpc::PyClient*>();
    py_client->set_service_info(service_info);
  }

  void Connect(const IPEndPoint& ip_endpoint, StatusOnceCallback callback) {
    rpc::PyClient* py_client = client_.cast<rpc::PyClient*>();
    internal::StatusOnceCallbackHolder* callback_holder =
        new internal::StatusOnceCallbackHolder(std::move(callback));
    auto py_callback = [callback_holder](const Status& s) {
      callback_holder->Invoke(s);
    };
    return py_client->Connect(ip_endpoint, py_callback);
  }

  Status Run() {
    rpc::PyClient* py_client = client_.cast<rpc::PyClient*>();
    return py_client->Run();
  }

  Status Shutdown() {
    rpc::PyClient* py_client = client_.cast<rpc::PyClient*>();
    return py_client->Shutdown();
  }

 private:
  py::object client_;
};

class PyServiceClient : public ServiceClient<PyClientBridge> {
 public:
  explicit PyServiceClient(py::object client) {
    client_ = PyClientBridge(client);
  }

  void RequestRegister(const NodeInfo& node_info, const std::string& service,
                       py::function py_on_connect_callback = py::none(),
                       py::function py_callback = py::none()) {
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

  void RequestUnregister(const NodeInfo& node_info, const std::string& service,
                         py::function py_callback = py::none()) {
    StatusOnceCallback callback;
    if (!py_callback.is_none()) {
      callback = base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(py_callback)));
    }
    ServiceClient<PyClientBridge>::RequestUnregister(node_info, service,
                                                     std::move(callback));
  }
};

class PyServerBridge {
 public:
  PyServerBridge() = default;
  explicit PyServerBridge(py::object server) : server_(server) {}

  ChannelDef channel_def() const {
    rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
    return py_server->channel_def();
  }

  void set_service_info(const ServiceInfo& service_info) {
    rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
    return py_server->set_service_info(service_info);
  }

  Status Start() {
    rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
    return py_server->Start();
  }

  Status Run() {
    rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
    return py_server->Run();
  }

  Status Shutdown() {
    rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
    return py_server->Shutdown();
  }

  std::string service_type() const {
    rpc::PyServer* py_server = server_.cast<rpc::PyServer*>();
    return py_server->service_type();
  }

 private:
  py::object server_;
};

struct EmptyService {};

class PyServiceServer : public ServiceServer<EmptyService, PyServerBridge> {
 public:
  explicit PyServiceServer(py::object server) {
    server_ = PyServerBridge(server);
  }

  void RequestRegister(const NodeInfo& node_info, const std::string& service,
                       py::function py_callback = py::none()) {
    StatusOnceCallback callback;
    if (!py_callback.is_none()) {
      callback = base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(py_callback)));
    }
    ServiceServer<EmptyService, PyServerBridge>::RequestRegister(
        node_info, service, std::move(callback));
  }

  void RequestUnregister(const NodeInfo& node_info, const std::string& service,
                         py::function py_callback = py::none()) {
    StatusOnceCallback callback;
    if (!py_callback.is_none()) {
      callback = base::BindOnce(&PyStatusCallback::Invoke,
                                base::Owned(new PyStatusCallback(py_callback)));
    }
    ServiceServer<EmptyService, PyServerBridge>::RequestUnregister(
        node_info, service, std::move(callback));
  }
};

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

  py::class_<PySerializedMessagePublisher>(communication, "Publisher")
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

  py::class_<PySerializedMessageSubscriber>(communication, "Subscriber")
      .def(py::init<py::object, TopicInfo::ImplType>(), py::arg("message_type"),
           py::arg("impl_type") = TopicInfo::PROTOBUF)
      .def("is_registering", &PySerializedMessageSubscriber::IsRegistering)
      .def("is_registered", &PySerializedMessageSubscriber::IsRegistered)
      .def("is_unregistering", &PySerializedMessageSubscriber::IsUnregistering)
      .def("is_unregistered", &PySerializedMessageSubscriber::IsUnregistered)
      .def("is_started", &PySerializedMessageSubscriber::IsStarted)
      .def("is_stopped", &PySerializedMessageSubscriber::IsStopped)
      .def("request_subscribe",
           [](PySerializedMessageSubscriber& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              const communication::Settings& settings,
              py::function on_message_callback) {
             self.RequestSubscribe(node_info, topic, channel_types, settings,
                                   on_message_callback, py::none(), py::none());
           },
           py::arg("node_info"), py::arg("topic"), py::arg("channel_types"),
           py::arg("settings"), py::arg("on_message_callback"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_subscribe",
           [](PySerializedMessageSubscriber& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              const communication::Settings& settings,
              py::function on_message_callback,
              py::function on_message_error_callback) {
             self.RequestSubscribe(node_info, topic, channel_types, settings,
                                   on_message_callback,
                                   on_message_error_callback, py::none());
           },
           py::arg("node_info"), py::arg("topic"), py::arg("channel_types"),
           py::arg("settings"), py::arg("on_message_callback"),
           py::arg("on_message_error_callback").none(true),
           py::call_guard<py::gil_scoped_release>())
      .def("request_subscribe",
           [](PySerializedMessageSubscriber& self, const NodeInfo& node_info,
              const std::string& topic, int channel_types,
              const communication::Settings& settings,
              py::function on_message_callback,
              py::function on_message_error_callback, py::function callback) {
             self.RequestSubscribe(node_info, topic, channel_types, settings,
                                   on_message_callback,
                                   on_message_error_callback, callback);
           },
           py::arg("node_info"), py::arg("topic"), py::arg("channel_types"),
           py::arg("settings"), py::arg("on_message_callback"),
           py::arg("on_message_error_callback").none(true),
           py::arg("callback").none(true),
           py::call_guard<py::gil_scoped_release>())
      .def("request_unsubscribe",
           [](PySerializedMessageSubscriber& self, const NodeInfo& node_info,
              const std::string& topic) {
             self.RequestUnsubscribe(node_info, topic, py::none());
           },
           py::arg("node_info"), py::arg("topic"),
           py::call_guard<py::gil_scoped_release>())
      .def("request_unsubscribe",
           [](PySerializedMessageSubscriber& self, const NodeInfo& node_info,
              const std::string& topic, py::function callback) {
             self.RequestUnsubscribe(node_info, topic, callback);
           },
           py::arg("node_info"), py::arg("topic"),
           py::arg("callback").none(true),
           py::call_guard<py::gil_scoped_release>());

  py::class_<PyServiceClient>(communication, "ServiceClient")
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

  py::class_<PyServiceServer>(communication, "ServiceServer")
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