#include "pybind11/pybind11.h"

#include "felicia/core/lib/error/status.h"

namespace py = pybind11;

namespace felicia {

PYBIND11_MODULE(status, m) {
  m.doc() = "Bindings for Status.";

  py::class_<Status>(m, "Status")
      .def(py::init<>(), "Create a success status.")
      .def_static("OK", &Status::OK, "Convenience static method.")
      .def("error_code", &Status::error_code)
      .def("error_message", &Status::error_message)
      .def("ok", &Status::ok);
}

}  // namespace felicia
