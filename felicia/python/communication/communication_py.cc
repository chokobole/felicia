#include "felicia/python/communication/communication_py.h"

#include "felicia/python/communication/ros_serialized_service_client_py.h"
#include "felicia/python/communication/ros_serialized_service_server_py.h"
#include "felicia/python/communication/serialized_message_publisher_py.h"
#include "felicia/python/communication/serialized_message_subscriber_py.h"
#include "felicia/python/communication/service_client_py.h"
#include "felicia/python/communication/service_server_py.h"

namespace felicia {

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

  AddSerializedMessagePublisher(communication);
  AddSerializedMessageSubscriber(communication);
  AddServiceClient(communication);
  AddServiceServer(communication);
#if defined(HAS_ROS)
  AddRosSerializedServiceClient(communication);
  AddRosSerializedServiceServer(communication);
#endif  // defined(HAS_ROS)
}

}  // namespace felicia