#include "felicia/drivers/camera/depth_camera_frame.h"

namespace felicia {

DepthCameraFrame::DepthCameraFrame(std::unique_ptr<uint8_t[]> data,
                                   size_t length, CameraFormat camera_format,
                                   float scale)
    : CameraFrame(std::move(data), length, camera_format), scale_(scale) {}

DepthCameraFrame::DepthCameraFrame(DepthCameraFrame&& other) noexcept
    : CameraFrame(std::move(other)), scale_(other.scale_) {}

DepthCameraFrame& DepthCameraFrame::operator=(DepthCameraFrame&& other) {
  CameraFrame::operator=(std::move(other));
  scale_ = other.scale_;

  return *this;
}

DepthCameraFrame::~DepthCameraFrame() = default;

float DepthCameraFrame::scale() const { return scale_; }

void DepthCameraFrame::set_scale(float scale) { scale_ = scale; }

DepthCameraFrameMessage DepthCameraFrame::ToDepthCameraFrameMessage() const {
  DepthCameraFrameMessage message;

  *message.mutable_frame() = ToCameraFrameMessage();
  message.set_scale(scale_);

  return message;
}

}  // namespace felicia