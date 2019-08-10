#ifndef FELICIA_PYTHON_DRIVERS_CAMERA_PY_H_
#define FELICIA_PYTHON_DRIVERS_CAMERA_PY_H_

#include "pybind11/pybind11.h"

#include "felicia/drivers/camera/camera_interface.h"

namespace py = pybind11;

namespace felicia {
namespace drivers {

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

  Status SetCameraSettings(const CameraSettings& camera_settings) override {
    PYBIND11_OVERLOAD(Status,            /* Return type */
                      CameraInterface,   /* Parent class */
                      SetCameraSettings, /* Name of function in C++ (must match
                                            Python name) */
                      camera_settings    /* Argument(s) */
    );
  }

  Status GetCameraSettingsInfo(
      CameraSettingsInfoMessage* camera_settings) override {
    PYBIND11_OVERLOAD(Status,                /* Return type */
                      CameraInterface,       /* Parent class */
                      GetCameraSettingsInfo, /* Name of function in C++ (must
                                                match Python name) */
                      camera_settings        /* Argument(s) */
    );
  }
};

void AddCamera(py::module& m);

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_PYTHON_DRIVERS_CAMERA_PY_H_