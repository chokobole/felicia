#ifndef FELICIA_DRIVERS_CAMERA_FAKE_CAMERA_H_
#define FELICIA_DRIVERS_CAMERA_FAKE_CAMERA_H_

#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {

class FakeCamera : public CameraInterface {
 public:
  FakeCamera(const CameraDescriptor& descriptor) {}

  Status Init() override { return Status::OK(); }
  Status Start(CameraFrameCallback camera_frame_callback,
               StatusCallback status_callback) override {
    return Status::OK();
  }
  Status Stop() override { return Status::OK(); }

  StatusOr<CameraFormat> GetCurrentCameraFormat() override {
    return CameraFormat();
  }
  Status SetCameraFormat(const CameraFormat& format) override {
    return Status::OK();
  }
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_FAKE_CAMERA_H_