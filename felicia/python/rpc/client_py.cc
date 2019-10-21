#include "felicia/python/rpc/client_py.h"

#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {
namespace rpc {

void AddClient(py::module& m) {
  py::class_<PyClientInterface, PyClient>(m, "_Client")
      .def(py::init<>())
      .def("service_info", &PyClientInterface::service_info);
}

}  // namespace rpc
}  // namespace felicia