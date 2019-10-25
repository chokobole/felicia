#ifndef FELICIA_PYTHON_RPC_SERVER_PY_H_
#define FELICIA_PYTHON_RPC_SERVER_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/core/rpc/server_interface.h"
#include "felicia/python/type_conversion/util.h"

namespace py = pybind11;

namespace felicia {
namespace rpc {

class PyServer : public ServerInterface {
 public:
  using ServerInterface::ServerInterface;

  Status Start() override {
    PYBIND11_OVERLOAD_INT(Status, ServerInterface, "start");
    FEL_CALL_PURE_FUNCTION(ServerInterface, "Start");
  }

  Status Run() override { return Status::OK(); }

  Status Shutdown() override {
    PYBIND11_OVERLOAD_INT(Status, ServerInterface, "shutdown");
    FEL_CALL_PURE_FUNCTION(ServerInterface, "Shutdown");
  }

  std::string GetServiceTypeName() const override {
    PYBIND11_OVERLOAD_INT(std::string, ServerInterface,
                          "get_service_type_name");
    FEL_CALL_PURE_FUNCTION(ServerInterface, "GetServiceTypeName");
  }
};

class PyServerBridge {
 public:
  PyServerBridge();
  explicit PyServerBridge(py::object server);

  ChannelDef channel_def() const;

  void set_service_info(const ServiceInfo& service_info);

  Status Start();

  Status Run();

  Status Shutdown();

  std::string GetServiceTypeName() const;

 private:
  py::object server_;
};

void AddServer(py::module& m);

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_SERVER_PY_H_