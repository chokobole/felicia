#ifndef FELICIA_CORE_DRIVERS_CAMERA_CAMERA_INTERFACE_H_
#define FELICIA_CORE_DRIVERS_CAMERA_CAMERA_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"

namespace felicia {

class EXPORT CameraInterface {
 public:
  CameraInterface();
  virtual ~CameraInterface();

  virtual Status Init() = 0;
  virtual Status Start() = 0;
  virtual Status Close() = 0;

  virtual Status TakePhoto() = 0;
};

}  // namespace felicia

#endif  // FELICIA_CORE_DRIVERS_CAMERA_CAMERA_INTERFACE_H_