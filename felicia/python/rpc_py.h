#ifndef FELICIA_PYTHON_RPC_PY_H_
#define FELICIA_PYTHON_RPC_PY_H_

#include "pybind11/pybind11.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/rpc/client.h"
#include "felicia/core/rpc/server_interface.h"

namespace py = pybind11;

namespace felicia {
namespace rpc {

struct EmptyService {};

template <>
class Client<EmptyService> {
 public:
  Client() = default;
  virtual ~Client() = default;

  virtual Status ConnectAndRun(const IPEndPoint& ip_endpoint) = 0;

  virtual Status Shutdown() = 0;

  DISALLOW_COPY_AND_ASSIGN(Client);
};

class PyClient : public Client<EmptyService> {
 public:
  using Client<EmptyService>::Client;

  Status ConnectAndRun(const IPEndPoint& ip_endpoint) override {
    PYBIND11_OVERLOAD_PURE(
        Status,               /* Return type */
        Client<EmptyService>, /* Parent class */
        ConnectAndRun, /* Name of function in C++ (must match Python name) */
        ip_endpoint    /* Argument(s) */
    );
  }

  Status Shutdown() override {
    PYBIND11_OVERLOAD_PURE(
        Status,               /* Return type */
        Client<EmptyService>, /* Parent class */
        Shutdown, /* Name of function in C++ (must match Python name) */
    );
  }
};

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

  std::string service_name() const override {
    PYBIND11_OVERLOAD_PURE(
        std::string,     /* Return type */
        ServerInterface, /* Parent class */
        service_name,    /* Name of function in C++ (must match Python name) */
    );
  }
};

void AddRpc(py::module& m);

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_PY_H_