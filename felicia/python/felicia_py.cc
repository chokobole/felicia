#include "pybind11/pybind11.h"

#include "felicia/core/felicia_init.h"
#include "felicia/python/command_line_interface/flag_py.h"
#include "felicia/python/master_proxy_py.h"
#include "felicia/python/node_py.h"

namespace py = pybind11;

namespace felicia {

extern void AddCommunication(py::module& m);

void AddGlobalFunctions(py::module& m) { m.def("felicia_init", &FeliciaInit); }

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

  AddGlobalFunctions(m);
  AddStatus(m);
  AddMasterProxy(m);
  AddNodeLifecycle(m);
  AddCommunication(m);

  py::module command_line_interface = m.def_submodule("command_line_interface");
  AddFlag(command_line_interface);
}

}  // namespace felicia