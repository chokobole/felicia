#ifndef FELICIA_PYTHON_RPC_SERVER_PY_H_
#define FELICIA_PYTHON_RPC_SERVER_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/core/rpc/server_interface.h"

namespace py = pybind11;

namespace felicia {
namespace rpc {

class PyServer : public ServerInterface {
 public:
  using ServerInterface::ServerInterface;

  Status Start() override {
    PYBIND11_OVERLOAD_PURE(
        Status,          /* Return type */
        ServerInterface, /* Parent class */
        Start,           /* Name of function in C++ (must match Python name) */
    );
  }

  Status Run() override {
    PYBIND11_OVERLOAD_PURE(
        Status,          /* Return type */
        ServerInterface, /* Parent class */
        Run,             /* Name of function in C++ (must match Python name) */
    );
  }

  Status Shutdown() override {
    PYBIND11_OVERLOAD_PURE(
        Status,          /* Return type */
        ServerInterface, /* Parent class */
        Shutdown,        /* Name of function in C++ (must match Python name) */
    );
  }

  std::string service_type() const override {
    PYBIND11_OVERLOAD_PURE(
        std::string,     /* Return type */
        ServerInterface, /* Parent class */
        service_type,    /* Name of function in C++ (must match Python name) */
    );
  }
};

void AddServer(py::module& m);

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_SERVER_PY_H_