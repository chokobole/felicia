#ifndef FELICIA_CORE_DRIVERS_CAMERA_CAMERA_INTERFACE_H_
#define FELICIA_CORE_DRIVERS_CAMERA_CAMERA_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/camera/camera_frame.h"

namespace felicia {

class EXPORT CameraInterface {
 public:
  CameraInterface();
  virtual ~CameraInterface();

  virtual Status Init() = 0;
  virtual Status Start(CameraFrameCallback camera_frame_callback,
                       StatusCallback status_callback) = 0;
  virtual Status Close() = 0;

  virtual Status GetSupportedCameraFormats(CameraFormats* camera_formats) = 0;
  virtual StatusOr<CameraFormat> GetCurrentCameraFormat() = 0;
  virtual Status SetCameraFormat(const CameraFormat& format) = 0;

 protected:
  CameraFrameCallback camera_frame_callback_;
  StatusCallback status_callback_;
};

}  // namespace felicia

#endif  // FELICIA_CORE_DRIVERS_CAMERA_CAMERA_INTERFACE_H_