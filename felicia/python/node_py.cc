#include "felicia/python/node_py.h"
#include "felicia/python/type_conversion/protobuf.h"

SUPPORT_PROTOBUF_TYPE_CAST(::felicia::NodeInfo, NodeInfo,
                           felicia.core.protobuf.master_data_pb2)

namespace felicia {

void AddNodeLifecycle(py::module& m) {
  py::class_<NodeLifecycle, PyNodeLifecycle>(m, "_NodeLifecycle")
      .def(py::init<>())
      .def("OnInit", &NodeLifecycle::OnInit,
           "It is called before created, each node has to initialize.")
      .def("OnDidCreate", &NodeLifecycle::OnDidCreate,
           "It is called when succeeded to be created.")
      .def("OnError", &NodeLifecycle::OnError,
           "It is called when error happens");
}

}  // namespace felicia