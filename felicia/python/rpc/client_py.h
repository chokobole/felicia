#ifndef FELICIA_PYTHON_RPC_CLIENT_PY_H_
#define FELICIA_PYTHON_RPC_CLIENT_PY_H_

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"

#include "felicia/core/rpc/client_interface.h"
#include "felicia/python/type_conversion/protobuf.h"

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
                       std::function<void(Status)> callback) = 0;
};

class PyClient : public PyClientInterface {
 public:
  using PyClientInterface::PyClientInterface;

  void Connect(const IPEndPoint& ip_endpoint,
               std::function<void(Status)> callback) override {
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

class PyClientBridge {
 public:
  PyClientBridge();
  explicit PyClientBridge(py::object client);

  void set_service_info(const ServiceInfo& service_info);

  void Connect(const IPEndPoint& ip_endpoint, StatusOnceCallback callback);

  Status Run();

  Status Shutdown();

 private:
  py::object client_;
};

void AddClient(py::module& m);

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_CLIENT_PY_H_