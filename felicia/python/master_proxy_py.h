#include "pybind11/pybind11.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/protobuf/master.pb.h"

namespace py = pybind11;

namespace felicia {

// This class is a wrapper for ::felicia::MasterProxy to open apis for python
// side.
class PyMasterProxy {
 public:
  static Status Start();
  static Status Stop();

  static void Run();

  static void RequestRegisterNode(py::function constructor,
                                  const NodeInfo& node_info, py::args args,
                                  py::kwargs kwargs);
  static void OnRegisterNodeAsync(py::object object,
                                  RegisterNodeRequest* request,
                                  RegisterNodeResponse* response,
                                  const Status& s);
};

void AddMasterProxy(py::module& m);

}  // namespace felicia