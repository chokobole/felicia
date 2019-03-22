#ifndef FELICIA_PYTHON_NODE_PY_H_
#define FELICIA_PYTHON_NODE_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace py = pybind11;

namespace felicia {

class PyNodeLifecycle : public NodeLifecycle {
 public:
  using NodeLifecycle::NodeLifecycle;

  void OnInit() override {
    PYBIND11_OVERLOAD_PURE(
        void,          /* Return type */
        NodeLifecycle, /* Parent class */
        OnInit,        /* Name of function in C++ (must match Python name) */
    );
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    PYBIND11_OVERLOAD(
        void,          /* Return type */
        NodeLifecycle, /* Parent class */
        OnDidCreate,   /* Name of function in C++ (must match Python name) */
        node_info      /* Argument(s) */
    );
  }

  void OnError(const Status& status) override {
    PYBIND11_OVERLOAD(
        void,          /* Return type */
        NodeLifecycle, /* Parent class */
        OnError,       /* Name of function in C++ (must match Python name) */
        status         /* Argument(s) */
    );
  }
};

void AddNodeLifecycle(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_NODE_PY_H_