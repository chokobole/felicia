#ifndef FELICIA_PYTHON_NODE_NODE_LIFECYCLE_PY_H_
#define FELICIA_PYTHON_NODE_NODE_LIFECYCLE_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/core/node/node_lifecycle.h"
#include "felicia/core/protobuf/master_data.pb.h"

namespace py = pybind11;

namespace felicia {

class PyNodeLifecycle : public NodeLifecycle {
 public:
  using NodeLifecycle::NodeLifecycle;

  void OnInit() override {
    PYBIND11_OVERLOAD_INT(void, NodeLifecycle, "on_init");
    return NodeLifecycle::OnInit();
  }

  void OnDidCreate(const NodeInfo& node_info) override {
    PYBIND11_OVERLOAD_INT(void, NodeLifecycle, "on_did_create", node_info);
    return NodeLifecycle::OnDidCreate(node_info);
  }

  void OnError(const Status& status) override {
    PYBIND11_OVERLOAD_INT(void, NodeLifecycle, "on_error", status);
    return NodeLifecycle::OnError(status);
  }
};

void AddNodeLifecycle(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_NODE_NODE_LIFECYCLE_PY_H_