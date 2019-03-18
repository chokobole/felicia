#include "pybind11/pybind11.h"

#include "felicia/examples/learn/message_communication/protobuf/message_spec.pb.h"
#include "felicia/python/communication_py_helper.h"

SUPPORT_PROTOBUF_TYPE_CAST(
    ::felicia::MessageSpec, MessageSpec,
    felicia.examples.learn.message_communication.protobuf.message_spec_pb2)

namespace py = pybind11;

namespace felicia {

PYBIND11_MODULE(communication, m) {
  m.doc() = "Bindings for communication.";

  AddPublisher<MessageSpec>(m, "Publisher");
  AddSubscriber<MessageSpec>(m, "Subscriber");
}

}  // namespace felicia