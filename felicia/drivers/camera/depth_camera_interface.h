#ifndef FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_
#define FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/drivers/camera/camera_state.h"

namespace felicia {

class EXPORT DepthCameraInterface {
 public:
  DepthCameraInterface(const CameraDescriptor& camera_descriptor);
  virtual ~DepthCameraInterface();

  virtual Status Init() = 0;
  virtual Status Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       CameraFrameCallback color_frame_callback,
                       CameraFrameCallback depth_frame_callback,
                       StatusCallback status_callback) = 0;
  virtual Status Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       DepthCameraFrameCallback depth_camera_frame_callback,
                       StatusCallback status_callback) = 0;
  virtual Status Stop() = 0;

 protected:
  CameraDescriptor camera_descriptor_;
  CameraFormat color_format_;
  CameraFormat depth_format_;
  CameraState camera_state_;

  CameraFrameCallback color_frame_callback_;
  CameraFrameCallback depth_frame_callback_;
  DepthCameraFrameCallback depth_camera_frame_callback_;
  StatusCallback status_callback_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_