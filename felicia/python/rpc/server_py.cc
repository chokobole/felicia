#include "felicia/python/rpc/server_py.h"

#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {
namespace rpc {

void AddServer(py::module& m) {
  py::class_<ServerInterface, PyServer>(m, "_Server")
      .def(py::init<>())
      .def("ConfigureServerAddress",
           &ServerInterface::ConfigureServerAddress);
}

}  // namespace rpc
}  // namespace felicia