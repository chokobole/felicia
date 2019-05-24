#ifndef FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_FRAME_H_
#define FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_FRAME_H_

#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/drivers/camera/depth_camera_frame_message.pb.h"

namespace felicia {

class EXPORT DepthCameraFrame : public CameraFrame {
 public:
  DepthCameraFrame(std::unique_ptr<uint8_t[]> data, CameraFormat camera_format,
                   float scale);
  DepthCameraFrame(DepthCameraFrame&& other) noexcept;
  DepthCameraFrame& operator=(DepthCameraFrame&& other);
  ~DepthCameraFrame();

  float scale() const;
  void set_scale(float scale);

  DepthCameraFrameMessage ToDepthCameraFrameMessage() const;

 private:
  float scale_ = 0;

  DISALLOW_COPY_AND_ASSIGN(DepthCameraFrame);
};

typedef ::base::RepeatingCallback<void(DepthCameraFrame)>
    DepthCameraFrameCallback;
typedef ::base::RepeatingCallback<void(CameraFrame, DepthCameraFrame)>
    SynchedDepthCameraFrameCallback;

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_