#ifndef FELICIA_PYTHON_RPC_PY_H_
#define FELICIA_PYTHON_RPC_PY_H_

#include "pybind11/pybind11.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/rpc/client.h"
#include "felicia/core/rpc/server.h"

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

  DISALLOW_COPY_AND_ASSIGN(Client<EmptyService>);
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

template <>
class Server<EmptyService> {
 public:
  Server() = default;
  virtual ~Server() = default;

  ChannelDef channel_def() const;

  virtual Status Start() = 0;

  Status Run() { return Status::OK(); }

  virtual Status Shutdown() = 0;

  std::string ConfigureServerAddress();

  virtual std::string service_name() const = 0;

 private:
  uint16_t port_;

  DISALLOW_COPY_AND_ASSIGN(Server<EmptyService>);
};

class PyServer : public Server<EmptyService> {
 public:
  using Server<EmptyService>::Server;

  Status Start() override {
    PYBIND11_OVERLOAD_PURE(
        Status,               /* Return type */
        Server<EmptyService>, /* Parent class */
        Start, /* Name of function in C++ (must match Python name) */
    );
  }

  Status Shutdown() override {
    PYBIND11_OVERLOAD_PURE(
        Status,               /* Return type */
        Server<EmptyService>, /* Parent class */
        Shutdown, /* Name of function in C++ (must match Python name) */
    );
  }

  std::string service_name() const override {
    PYBIND11_OVERLOAD_PURE(
        std::string,          /* Return type */
        Server<EmptyService>, /* Parent class */
        service_name, /* Name of function in C++ (must match Python name) */
    );
  }
};

void AddRpc(py::module& m);

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_PY_H_