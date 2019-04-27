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
  AddPublisher<MessageSpec>(m, "Publisher");
  AddSubscriber<MessageSpec>(m, "Subscriber");
}

}  // namespace felicia