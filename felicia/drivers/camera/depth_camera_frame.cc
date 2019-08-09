#include "felicia/drivers/camera/depth_camera_frame.h"

namespace felicia {

DepthCameraFrame::DepthCameraFrame() = default;

DepthCameraFrame::DepthCameraFrame(CameraFrame other, float min, float max)
    : CameraFrame(std::move(other)), min_(min), max_(max) {}

DepthCameraFrame::DepthCameraFrame(DepthCameraFrame&& other) noexcept
    : CameraFrame(std::move(other)), min_(other.min_), max_(other.max_) {}

DepthCameraFrame& DepthCameraFrame::operator=(DepthCameraFrame&& other) {
  CameraFrame::operator=(std::move(other));
  min_ = other.min_;
  max_ = other.max_;

  return *this;
}

DepthCameraFrame::~DepthCameraFrame() = default;

DepthCameraFrameMessage DepthCameraFrame::ToDepthCameraFrameMessage() const {
  DepthCameraFrameMessage message;

  *message.mutable_frame() = ToCameraFrameMessage();
  message.set_min(min_);
  message.set_max(max_);

  return message;
}

// static
DepthCameraFrame DepthCameraFrame::FromDepthCameraFrameMessage(
    const DepthCameraFrameMessage& message) {
  return {CameraFrame::FromCameraFrameMessage(message.frame()), message.min(),
          message.max()};
}

// static
DepthCameraFrame DepthCameraFrame::FromDepthCameraFrameMessage(
    DepthCameraFrameMessage&& message) {
  float min = message.min();
  float max = message.max();
  return {CameraFrame::FromCameraFrameMessage(std::move(message.frame())), min,
          max};
}

#if defined(HAS_OPENCV)
// static
DepthCameraFrame DepthCameraFrame::FromCvMat(cv::Mat mat,
                                             const CameraFormat& camera_format,
                                             base::TimeDelta timestamp,
                                             float min, float max) {
  return {CameraFrame::FromCvMat(mat, camera_format, timestamp), min, max};
}
#endif

}  // namespace felicia