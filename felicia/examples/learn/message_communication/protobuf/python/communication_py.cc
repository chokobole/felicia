#include "pybind11/pybind11.h"

#include "felicia/examples/learn/message_communication/protobuf/message_spec.pb.h"
#include "felicia/python/communication_py_helper.h"
#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(
    ::felicia::MessageSpec, MessageSpec,
    felicia.examples.learn.message_communication.protobuf.message_spec_pb2)

namespace py = pybind11;

namespace felicia {

void AddCommunication(py::module& m) {
  py::module communication = m.def_submodule("communication");

  AddPublisher<MessageSpec>(communication, "Publisher");
  AddSubscriber<MessageSpec>(communication, "Subscriber");

  py::class_<communication::Settings>(communication, "Settings")
      .def(py::init<>())
      .def_readwrite("period", &communication::Settings::period)
      .def_readwrite("buffer_size", &communication::Settings::buffer_size)
      .def_readwrite("is_dynamic_buffer",
                     &communication::Settings::is_dynamic_buffer)
      .def_readwrite("queue_size", &communication::Settings::queue_size);
}

}  // namespace felicia
