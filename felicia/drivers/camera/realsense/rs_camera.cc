#include "felicia/drivers/camera/realsense/rs_camera.h"

#include "felicia/drivers/camera/camera_errors.h"

namespace felicia {

RsCamera::RsCamera(const CameraDescriptor& camera_descriptor)
    : DepthCameraInterface(camera_descriptor) {}

RsCamera::~RsCamera() = default;

Status RsCamera::Init() {
  return errors::Unimplemented("Not Implemented yet.");
}

// Status RsCamera::Start(CameraFrameCallback camera_frame_callback,
//                        StatusCallback status_callback) {
//   return errors::Unimplemented("Not Implemented yet.");
// }

// Status RsCamera::Stop() {
//   return errors::Unimplemented("Not Implemented yet.");
// }

// StatusOr<CameraFormat> RsCamera::GetCurrentCameraFormat() {
//   return errors::Unimplemented("Not Implemented yet.");
// }

// Status RsCamera::SetCameraFormat(const CameraFormat& camera_format) {
//   return errors::Unimplemented("Not Implemented yet.");
// }

}  // namespace felicia