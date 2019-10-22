#if defined(HAS_ROS)

#include "felicia/python/rpc/ros_serialized_server_py.h"

#include "third_party/chromium/base/memory/scoped_refptr.h"

namespace felicia {
namespace rpc {

PyRosSerializedServer::PyRosSerializedServer() = default;

PyRosSerializedServer::PyRosSerializedServer(py::object service)
    : Server<PyRosSerializedServiceBridge>(
          base::MakeRefCounted<PyRosSerializedServiceBridge>(service)) {}

}  // namespace rpc
}  // namespace felicia

#endif  // defined(HAS_ROS)