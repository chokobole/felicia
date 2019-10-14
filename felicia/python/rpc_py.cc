#include "felicia/python/rpc_py.h"

#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(felicia::IPEndPoint, IPEndPoint,
                           felicia.core.protobuf.channel_pb2)

namespace felicia {
namespace rpc {

ChannelDef Server<EmptyService>::channel_def() const {
  ChannelDef channel_def;
  channel_def.set_type(ChannelDef::CHANNEL_TYPE_TCP);
  IPEndPoint* ip_endpoint = channel_def.mutable_ip_endpoint();
  ip_endpoint->set_ip(HostIPAddress(HOST_IP_ONLY_ALLOW_IPV4).ToString());
  ip_endpoint->set_port(port_);
  return channel_def;
}

std::string Server<EmptyService>::ConfigureServerAddress() {
  port_ = PickRandomPort(true);
  return base::StringPrintf("[::]:%d", static_cast<int>(port_));
}

void AddRpc(py::module& m) {
  py::module rpc = m.def_submodule("rpc");

  py::class_<Client<EmptyService>, PyClient>(rpc, "_Client").def(py::init<>());

  py::class_<Server<EmptyService>, PyServer>(rpc, "_Server")
      .def(py::init<>())
      .def("ConfigureServerAddress",
           &Server<EmptyService>::ConfigureServerAddress);
}

}  // namespace rpc
}  // namespace felicia