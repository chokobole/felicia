// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(HAS_ROS)

#ifndef FELICIA_PYTHON_RPC_ROS_SERIALIZED_SERVICE_PY_H_
#define FELICIA_PYTHON_RPC_ROS_SERIALIZED_SERVICE_PY_H_

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"

#include "felicia/core/rpc/ros_serialized_service_interface.h"
#include "felicia/core/rpc/ros_service_manager.h"
#include "felicia/python/type_conversion/util.h"

namespace py = pybind11;

namespace felicia {
namespace rpc {

class PyRosSerializedServiceInterface {
 public:
  virtual ~PyRosSerializedServiceInterface() = default;

  virtual void Handle(const py::object request, py::object response,
                      std::function<void(Status)> callback) = 0;
};

class PyRosSerializedService : public PyRosSerializedServiceInterface {
 public:
  explicit PyRosSerializedService(py::object ros_service)
      : ros_service_(ros_service) {}

  py::object ros_service() const { return ros_service_; }

  void Handle(const py::object request, py::object response,
              std::function<void(Status)> callback) override {
    FEL_OVERLOAD_INT_WITH_GIL(void, PyRosSerializedServiceInterface, "handle",
                              request, response, callback);
    FEL_CALL_PURE_FUNCTION(PyRosSerializedServiceInterface, "Handle");
  }

 private:
  py::object ros_service_;
};

class PyRosSerializedServiceBridge : public RosSerializedServiceInterface {
 public:
  PyRosSerializedServiceBridge();
  explicit PyRosSerializedServiceBridge(py::object service);
  ~PyRosSerializedServiceBridge() override;

  void Handle(const SerializedMessage* request, SerializedMessage* response,
              StatusOnceCallback callback) override;

 private:
  py::object service_;
  py::object request_type_;
  py::object response_type_;
};

void AddRosSerializedService(py::module& m);

}  // namespace rpc
}  // namespace felicia

#endif  // FELICIA_PYTHON_RPC_ROS_SERIALIZED_SERVICE_PY_H_

#endif  // defined(HAS_ROS)