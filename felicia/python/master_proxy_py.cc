#include "pybind11/pybind11.h"

#include "felicia/core/master/master_data.pb.h"
#include "felicia/python/core/lib/type_conversion/protobuf.h"
#include "felicia/python/py_master_proxy.h"

SUPPORT_PROTOBUF_TYPE_CAST(::felicia::NodeInfo, NodeInfo,
                           felicia.core.master.master_data_pb2)

namespace py = pybind11;

namespace felicia {

PYBIND11_MODULE(master_proxy, m) {
  m.doc() = "Bindings for MasterProxy.";

  py::module::import("felicia.python.core.lib.error.status").attr("Status");

  py::class_<PyMasterProxy>(m, "MasterProxy")
      .def_static("start", &PyMasterProxy::Start)
      .def_static("stop", &PyMasterProxy::Stop)
      .def_static("run", &PyMasterProxy::Run)
      .def_static("request_register_node", &PyMasterProxy::RequestRegisterNode);
}

}  // namespace felicia