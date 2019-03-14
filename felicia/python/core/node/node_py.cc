#include "pybind11/pybind11.h"

#include "felicia/core/node/node_lifecycle.h"
#include "felicia/python/core/node/py_node_lifecycle.h"

namespace py = pybind11;

namespace felicia {

PYBIND11_MODULE(node, m) {
  m.doc() = "Bindings for Node.";

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
