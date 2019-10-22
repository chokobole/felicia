#if defined(HAS_ROS)

#ifndef FELICIA_PYTHON_RPC_SERIALIZED_CLIENT_PY_H_
#define FELICIA_PYTHON_RPC_SERIALIZED_CLIENT_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/core/rpc/client.h"

namespace py = pybind11;

namespace felicia {
namespace rpc {

class PyRosSerializedClient : public Client<RosSerializedService> {
 public:
  PyRosSerializedClient();
  explicit PyRosSerializedClient(py::object ros_service);

 private:
  std::string GetServiceTypeName() const { return service_type_name_; }

  std::string GetServiceMD5Sum() const { return service_md5sum_; }

  std::string service_type_name_;
  std::string service_md5sum_;
};

void AddRosSerializedClient(py::module& m);

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_SERIALIZED_CLIENT_PY_H_

#endif  // defined(HAS_ROS)