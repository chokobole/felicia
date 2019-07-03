#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_INTERFACE_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_INTERFACE_H_

#include "felicia/drivers/camera/camera_interface_base.h"

namespace felicia {

class EXPORT CameraInterface : public CameraInterfaceBase {
 public:
  CameraInterface(const CameraDescriptor& camera_descriptor);

  virtual Status Start(const CameraFormat& requested_camera_format,
                       CameraFrameCallback camera_frame_callback,
                       StatusCallback status_callback) = 0;

  CameraFormat camera_format() const;

 protected:
  CameraFormat camera_format_;
  PixelFormat requested_pixel_format_;

  CameraFrameCallback camera_frame_callback_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_INTERFACE_H_