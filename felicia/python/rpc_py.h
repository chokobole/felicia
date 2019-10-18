#ifndef FELICIA_PYTHON_RPC_PY_H_
#define FELICIA_PYTHON_RPC_PY_H_

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/rpc/client.h"
#include "felicia/core/rpc/server_interface.h"

namespace py = pybind11;

namespace felicia {
namespace rpc {

class PyClientInterface : public ClientInterface {
 public:
  const ServiceInfo& service_info() const { return service_info_; }

  void Connect(const IPEndPoint& ip_endpoint, StatusOnceCallback callback) {
    NOTREACHED();
  }

  virtual void Connect(const IPEndPoint& ip_endpoint,
                       std::function<void(const Status& s)> callback) = 0;
};

class PyClient : public PyClientInterface {
 public:
  using PyClientInterface::PyClientInterface;

  void Connect(const IPEndPoint& ip_endpoint,
               std::function<void(const Status& s)> callback) override {
    PYBIND11_OVERLOAD_PURE(
        void,              /* Return type */
        PyClientInterface, /* Parent class */
        Connect,     /* Name of function in C++ (must match Python name) */
        ip_endpoint, /* Argument(s) */
        callback);
  }

  Status Run() override {
    PYBIND11_OVERLOAD_PURE(
        Status,            /* Return type */
        PyClientInterface, /* Parent class */
        Run, /* Name of function in C++ (must match Python name) */
    );
  }

  Status Shutdown() override {
    PYBIND11_OVERLOAD_PURE(
        Status,            /* Return type */
        PyClientInterface, /* Parent class */
        Shutdown, /* Name of function in C++ (must match Python name) */
    );
  }
};

class PyServer : public ServerInterface {
 public:
  using ServerInterface::ServerInterface;

  void set_use_ros_channel(bool use_ros_channel) override {}

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