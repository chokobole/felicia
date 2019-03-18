#include "pybind11/pybind11.h"

#include "felicia/core/communication/subscriber.h"

namespace py = pybind11;

namespace felicia {

PYBIND11_MODULE(communication, m) {
  m.doc() = "Bindings for communication.";

  py::class_<communication::Settings>(m, "Settings")
      .def(py::init<uint32_t, uint8_t>(), py::arg("period") = 1000,
           py::arg("queue_size") = 100);
}

}  // namespace felicia