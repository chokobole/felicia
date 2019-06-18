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

}  // namespace felicia