#if defined(HAS_ROS)

#ifndef FELICIA_PYTHON_RPC_ROS_SERIALIZED_SERVICE_PY_H_
#define FELICIA_PYTHON_RPC_ROS_SERIALIZED_SERVICE_PY_H_

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"

#include "felicia/core/rpc/ros_serialized_service_interface.h"

namespace py = pybind11;

namespace felicia {
namespace rpc {

class PyRosSerializedServiceInterface {
 public:
  virtual void Handle(const py::object request, py::object response,
                      std::function<void(const Status&)> callback) = 0;
};

class PyRosSerializedService : public PyRosSerializedServiceInterface {
 public:
  explicit PyRosSerializedService(py::object ros_service)
      : ros_service_(ros_service) {}

  py::object ros_service() const { return ros_service_; }

  void Handle(const py::object request, py::object response,
              std::function<void(const Status&)> callback) override {
    PYBIND11_OVERLOAD_PURE(
        void,                            /* Return type */
        PyRosSerializedServiceInterface, /* Parent class */
        Handle,  /* Name of function in C++ (must match Python name) */
        request, /* Argument(s) */
        response, callback);
  }

 private:
  py::object ros_service_;
};

class PyRosSerializedServiceBridge : public RosSerializedServiceInterface {
 public:
  PyRosSerializedServiceBridge();
  explicit PyRosSerializedServiceBridge(py::object service);

  void Handle(const SerializedMessage* request, SerializedMessage* response,
              StatusOnceCallback callback) override;

 protected:
  ~PyRosSerializedServiceBridge() override;

  py::object service_;
  py::object request_type_;
  py::object response_type_;
};

void AddRosSerializedService(py::module& m);

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_ROS_SERIALIZED_SERVICE_PY_H_

#endif  // defined(HAS_ROS)