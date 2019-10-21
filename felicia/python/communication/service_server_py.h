#ifndef FELICIA_PYTHON_COMMUNICATION_SERVICE_SERVER_PY_H_
#define FELICIA_PYTHON_COMMUNICATION_SERVICE_SERVER_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/core/communication/service_server.h"
#include "felicia/python/rpc/server_py.h"

namespace py = pybind11;

namespace felicia {

class PyServerBridge {
 public:
  PyServerBridge();
  explicit PyServerBridge(py::object server);

  ChannelDef channel_def() const;

  void set_service_info(const ServiceInfo& service_info);

  Status Start();

  Status Run();

  Status Shutdown();

  std::string service_type() const;

 private:
  py::object server_;
};

struct EmptyService {};

class PyServiceServer : public ServiceServer<EmptyService, PyServerBridge> {
 public:
  explicit PyServiceServer(py::object server);

  void RequestRegister(const NodeInfo& node_info, const std::string& service,
                       py::function py_callback = py::none());

  void RequestUnregister(const NodeInfo& node_info, const std::string& service,
                         py::function py_callback = py::none());
};

void AddServiceServer(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_COMMUNICATION_SERVICE_SERVER_PY_H_