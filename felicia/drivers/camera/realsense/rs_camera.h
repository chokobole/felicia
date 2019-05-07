#ifndef FELICIA_DRIVERS_CAMERA_REALSENSE_RS_CAMERA_H_
#define FELICIA_DRIVERS_CAMERA_REALSENSE_RS_CAMERA_H_

#include <librealsense2/rs.hpp>

#include "felicia/drivers/camera/depth_camera_interface.h"

namespace felicia {

class RsCamera : public DepthCameraInterface {
 public:
  ~RsCamera();

  Status Init() override;
  // Status Start(CameraFrameCallback camera_frame_callback,
  //              StatusCallback status_callback);
  // Status Stop();

  // StatusOr<CameraFormat> GetCurrentCameraFormat();
  // Status SetCameraFormat(const CameraFormat& format);

 private:
  friend class RsCameraFactory;

  RsCamera(const CameraDescriptor& camera_descriptor);

  CameraDescriptor camera_descriptor_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(RsCamera);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_REALSENSE_RS_CAMERA_H_