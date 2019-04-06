#ifndef FELICIA_CORE_DRIVERS_CAMERA_CAMERA_INTERFACE_H_
#define FELICIA_CORE_DRIVERS_CAMERA_CAMERA_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/camera/camera_frame.h"

namespace felicia {

class EXPORT CameraInterface {
 public:
  CameraInterface();
  virtual ~CameraInterface();

  virtual Status Init() = 0;
  virtual Status Start(CameraFrameCallback callback) = 0;
  virtual Status Close() = 0;

  virtual StatusOr<CameraFormat> GetFormat() = 0;
  virtual Status SetFormat(CameraFormat format) = 0;
};

}  // namespace felicia

#endif  // FELICIA_CORE_DRIVERS_CAMERA_CAMERA_INTERFACE_H_