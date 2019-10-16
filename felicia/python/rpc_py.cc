#include "felicia/python/rpc_py.h"

#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(felicia::IPEndPoint, IPEndPoint,
                           felicia.core.protobuf.channel_pb2)

namespace felicia {
namespace rpc {

void AddRpc(py::module& m) {
  py::module rpc = m.def_submodule("rpc");

  py::class_<Client<EmptyService>, PyClient>(rpc, "_Client").def(py::init<>());

  py::class_<ServerInterface, PyServer>(rpc, "_Server")
      .def(py::init<>())
      .def("ConfigureServerAddress", &ServerInterface::ConfigureServerAddress);
}

}  // namespace rpc
}  // namespace felicia