#include "pybind11/pybind11.h"

#include "felicia/python/command_line_interface/flag_py.h"
#include "felicia/python/master_proxy_py.h"
#include "felicia/python/node_py.h"
#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(::felicia::NodeInfo, NodeInfo,
                           felicia.core.protobuf.master_data_pb2)
SUPPORT_PROTOBUF_TYPE_CAST(::felicia::ChannelDef, ChannelDef,
                           felicia.core.protobuf.channel_pb2)

namespace py = pybind11;

namespace felicia {

extern void AddCommunication(py::module& m);

void AddStatus(py::module& m) {
  py::class_<Status>(m, "Status")
      .def(py::init<>(), "Create a success status.")
      .def_static("OK", &Status::OK, "Convenience static method.")
      .def("error_code", &Status::error_code)
      .def("error_message", &Status::error_message)
      .def("ok", &Status::ok);
}
PYBIND11_MODULE(felicia_py, m) {
  m.doc() = "Bindings for Felicia.";

  AddStatus(m);
  AddMasterProxy(m);
  AddNodeLifecycle(m);
  AddCommunication(m);

  py::module command_line_interface = m.def_submodule("command_line_interface");
  AddFlag(command_line_interface);
}

}  // namespace felicia