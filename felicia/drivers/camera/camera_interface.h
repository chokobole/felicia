#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_INTERFACE_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/drivers/camera/camera_state.h"

namespace felicia {

class EXPORT CameraInterface {
 public:
  CameraInterface(const CameraDescriptor& camera_descriptor);
  virtual ~CameraInterface();

  virtual Status Init() = 0;
  virtual Status Start(const CameraFormat& requested_camera_format,
                       CameraFrameCallback camera_frame_callback,
                       StatusCallback status_callback) = 0;
  virtual Status Stop() = 0;

  bool IsInitialized() const;
  bool IsStarted() const;
  bool IsStopped() const;

 protected:
  CameraDescriptor camera_descriptor_;
  CameraFormat camera_format_;
  CameraState camera_state_;

  CameraFrameCallback camera_frame_callback_;
  StatusCallback status_callback_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_INTERFACE_H_