#ifndef FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_
#define FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_

#include "felicia/drivers/camera/camera_interface_base.h"
#include "felicia/drivers/camera/depth_camera_frame.h"

namespace felicia {

enum AlignDirection {
  None,
  AlignToDepth,
  AlignToColor,
};

class EXPORT DepthCameraInterface : public CameraInterfaceBase {
 public:
  DepthCameraInterface(const CameraDescriptor& camera_descriptor);

  virtual Status Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       CameraFrameCallback color_frame_callback,
                       DepthCameraFrameCallback depth_frame_callback,
                       StatusCallback status_callback) = 0;
  virtual Status Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       AlignDirection align_direction,
                       SynchedDepthCameraFrameCallback synched_frame_callback,
                       StatusCallback status_callback) = 0;

 protected:
  CameraFormat color_format_;
  CameraFormat depth_format_;

  CameraFrameCallback color_frame_callback_;
  DepthCameraFrameCallback depth_frame_callback_;
  SynchedDepthCameraFrameCallback synched_frame_callback_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_