#ifndef FELICIA_DRIVERS_CAMERA_STEREO_CAMERA_INTERFACE_H_
#define FELICIA_DRIVERS_CAMERA_STEREO_CAMERA_INTERFACE_H_

#include "felicia/drivers/camera/camera_interface_base.h"

namespace felicia {

class EXPORT StereoCameraInterface : public CameraInterfaceBase {
 public:
  StereoCameraInterface(const CameraDescriptor& camera_descriptor);

  virtual Status Start(const CameraFormat& requested_camera_format,
                       CameraFrameCallback left_camera_frame_callback,
                       CameraFrameCallback right_camera_frame_callback,
                       StatusCallback status_callback) = 0;

 protected:
  CameraFormat camera_format_;

  CameraFrameCallback left_camera_frame_callback_;
  CameraFrameCallback right_camera_frame_callback_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_STEREO_CAMERA_INTERFACE_H_