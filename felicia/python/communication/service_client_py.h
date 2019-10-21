#ifndef FELICIA_PYTHON_COMMUNICATION_SERVICE_CLIENT_PY_H_
#define FELICIA_PYTHON_COMMUNICATION_SERVICE_CLIENT_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/core/communication/service_client.h"
#include "felicia/python/rpc/client_py.h"

namespace py = pybind11;

namespace felicia {

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

class PyServiceClient : public ServiceClient<PyClientBridge> {
 public:
  explicit PyServiceClient(py::object client);

  void RequestRegister(const NodeInfo& node_info, const std::string& service,
                       py::function py_on_connect_callback = py::none(),
                       py::function py_callback = py::none());

  void RequestUnregister(const NodeInfo& node_info, const std::string& service,
                         py::function py_callback = py::none());
};

void AddServiceClient(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_COMMUNICATION_SERVICE_CLIENT_PY_H_