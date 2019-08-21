#ifndef FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_FRAME_H_
#define FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_FRAME_H_

#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/drivers/camera/depth_camera_frame_message.pb.h"

namespace felicia {
namespace drivers {

class EXPORT DepthCameraFrame : public CameraFrame {
 public:
  DepthCameraFrame();
  DepthCameraFrame(CameraFrame other, float min, float max);
  DepthCameraFrame(DepthCameraFrame&& other) noexcept;
  DepthCameraFrame& operator=(DepthCameraFrame&& other);
  ~DepthCameraFrame();

  DepthCameraFrameMessage ToDepthCameraFrameMessage(bool copy = true);
  Status FromDepthCameraFrameMessage(const DepthCameraFrameMessage& message);
  Status FromDepthCameraFrameMessage(DepthCameraFrameMessage&& message);

#if defined(HAS_OPENCV)
  Status FromCvMat(cv::Mat mat, const CameraFormat& camera_format,
                   base::TimeDelta timestamp, float min, float max);
#endif

 private:
  float min_;
  float max_;

  DISALLOW_COPY_AND_ASSIGN(DepthCameraFrame);
};

typedef base::RepeatingCallback<void(DepthCameraFrame&&)>
    DepthCameraFrameCallback;

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_