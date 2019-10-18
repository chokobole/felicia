#include "felicia/python/rpc_py.h"

#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(felicia::IPEndPoint, IPEndPoint,
                           felicia.core.protobuf.channel_pb2)
SUPPORT_PROTOBUF_TYPE_CAST(felicia::ServiceInfo, ServiceInfo,
                           felicia.core.protobuf.master_data)

namespace felicia {
namespace rpc {

void AddRpc(py::module& m) {
  py::module rpc = m.def_submodule("rpc");

  py::class_<PyClientInterface, PyClient>(rpc, "_Client")
      .def(py::init<>())
      .def("service_info", &PyClientInterface::service_info);

  py::class_<ServerInterface, PyServer>(rpc, "_Server")
      .def(py::init<>())
      .def("ConfigureServerAddress", &ServerInterface::ConfigureServerAddress);
}

}  // namespace rpc
}  // namespace felicia