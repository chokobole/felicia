#if defined(HAS_ROS)

#ifndef FELICIA_PYTHON_RPC_ROS_SERIALIZED_SERVER_PY_H_
#define FELICIA_PYTHON_RPC_ROS_SERIALIZED_SERVER_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/core/rpc/server.h"
#include "felicia/python/rpc/ros_serialized_service_py.h"

namespace py = pybind11;

namespace felicia {
namespace rpc {

class PyRosSerializedServer : public Server<PyRosSerializedServiceBridge> {
 public:
  PyRosSerializedServer();
  explicit PyRosSerializedServer(py::object service);
};

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_ROS_SERIALIZED_SERVER_PY_H_

#endif  // defined(HAS_ROS)