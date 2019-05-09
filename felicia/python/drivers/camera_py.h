#ifndef FELICIA_PYTHON_DRIVERS_CAMERA_PY_H_
#define FELICIA_PYTHON_DRIVERS_CAMERA_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/drivers/camera/camera_interface.h"

namespace py = pybind11;

namespace felicia {

class PyCameraInterface : public CameraInterface {
 public:
  using CameraInterface::CameraInterface;

  Status Init() override {
    PYBIND11_OVERLOAD_PURE(
        Status,          /* Return type */
        CameraInterface, /* Parent class */
        Init,            /* Name of function in C++ (must match Python name) */
    );
  }

  Status Start(const CameraFormat& requested_camera_format,
               CameraFrameCallback camera_frame_callback,
               StatusCallback status_callback) override {
    PYBIND11_OVERLOAD_PURE(
        Status,          /* Return type */
        CameraInterface, /* Parent class */
        Start,           /* Name of function in C++ (must match Python name) */
        requested_camera_format, /* Argument(s) */
        camera_frame_callback, status_callback);
  }

  Status Stop() override {
    PYBIND11_OVERLOAD_PURE(
        Status,          /* Return type */
        CameraInterface, /* Parent class */
        Stop,            /* Name of function in C++ (must match Python name) */
    );
  }
};

void AddCamera(py::module& m);

}  // namespace felicia

#endif  // FELICIA_PYTHON_DRIVERS_CAMERA_PY_H_