#ifndef FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_FRAME_H_
#define FELICIA_DRIVERS_CAMERA_DEPTH_CAMERA_FRAME_H_

#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/drivers/camera/depth_camera_frame_message.pb.h"

namespace felicia {
namespace drivers {

class FEL_EXPORT DepthCameraFrame : public CameraFrame {
 public:
  DepthCameraFrame();
  DepthCameraFrame(const CameraFrame& other, float min, float max);
  DepthCameraFrame(CameraFrame&& other, float min, float max);
  DepthCameraFrame(const DepthCameraFrame& other);
  DepthCameraFrame(DepthCameraFrame&& other) noexcept;
  DepthCameraFrame& operator=(const DepthCameraFrame& other);
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
};

typedef base::RepeatingCallback<void(DepthCameraFrame&&)>
    DepthCameraFrameCallback;

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_