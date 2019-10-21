#include "felicia/python/node/node_lifecycle_py.h"

#include "felicia/python/type_conversion/protobuf.h"

namespace felicia {

void AddNodeLifecycle(py::module& m) {
  py::class_<NodeLifecycle, PyNodeLifecycle>(m, "NodeLifecycle")
      .def(py::init<>());
}

}  // namespace felicia