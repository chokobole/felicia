#ifndef FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_
#define FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/drivers/camera/camera_state.h"
#include "felicia/drivers/camera/depth_camera_frame.h"

namespace felicia {

class EXPORT DepthCameraInterface {
 public:
  DepthCameraInterface(const CameraDescriptor& camera_descriptor);
  virtual ~DepthCameraInterface();

  virtual Status Init() = 0;
  virtual Status Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       CameraFrameCallback color_frame_callback,
                       DepthCameraFrameCallback depth_frame_callback,
                       StatusCallback status_callback) = 0;
  virtual Status Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       SynchedDepthCameraFrameCallback synched_frame_callback,
                       StatusCallback status_callback) = 0;
  virtual Status Stop() = 0;

  bool IsInitialized() const;
  bool IsStarted() const;
  bool IsStopped() const;

 protected:
  CameraDescriptor camera_descriptor_;
  CameraFormat color_format_;
  CameraFormat depth_format_;
  CameraState camera_state_;

  CameraFrameCallback color_frame_callback_;
  DepthCameraFrameCallback depth_frame_callback_;
  SynchedDepthCameraFrameCallback synched_frame_callback_;
  StatusCallback status_callback_;
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_INTERFACE_H_