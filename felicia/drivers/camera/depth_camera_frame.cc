#include "felicia/drivers/camera/depth_camera_frame.h"

namespace felicia {
namespace drivers {

DepthCameraFrame::DepthCameraFrame() = default;

DepthCameraFrame::DepthCameraFrame(const CameraFrame& other, float min,
                                   float max)
    : CameraFrame(other), min_(min), max_(max) {}

DepthCameraFrame::DepthCameraFrame(CameraFrame&& other, float min, float max)
    : CameraFrame(std::move(other)), min_(min), max_(max) {}

DepthCameraFrame::DepthCameraFrame(const DepthCameraFrame& other)
    : CameraFrame(other), min_(other.min_), max_(other.max_) {}

DepthCameraFrame::DepthCameraFrame(DepthCameraFrame&& other) noexcept
    : CameraFrame(std::move(other)), min_(other.min_), max_(other.max_) {}

DepthCameraFrame& DepthCameraFrame::operator=(const DepthCameraFrame& other) =
    default;

DepthCameraFrame& DepthCameraFrame::operator=(DepthCameraFrame&& other) =
    default;

DepthCameraFrame::~DepthCameraFrame() = default;

DepthCameraFrameMessage DepthCameraFrame::ToDepthCameraFrameMessage(bool copy) {
  DepthCameraFrameMessage message;

  *message.mutable_frame() = ToCameraFrameMessage(copy);
  message.set_min(min_);
  message.set_max(max_);

  return message;
}

Status DepthCameraFrame::FromDepthCameraFrameMessage(
    const DepthCameraFrameMessage& message) {
  CameraFrame camera_frame;
  Status s = camera_frame.FromCameraFrameMessage(message.frame());
  if (!s.ok()) return s;
  *this =
      DepthCameraFrame{std::move(camera_frame), message.min(), message.max()};
  return Status::OK();
}

Status DepthCameraFrame::FromDepthCameraFrameMessage(
    DepthCameraFrameMessage&& message) {
  float min = message.min();
  float max = message.max();
  CameraFrame camera_frame;
  Status s = camera_frame.FromCameraFrameMessage(std::move(message.frame()));
  if (!s.ok()) return s;
  *this = DepthCameraFrame{std::move(camera_frame), min, max};
  return Status::OK();
}

#if defined(HAS_OPENCV)
// static
Status DepthCameraFrame::FromCvMat(cv::Mat mat,
                                   const CameraFormat& camera_format,
                                   base::TimeDelta timestamp, float min,
                                   float max) {
  CameraFrame camera_frame;
  Status s = camera_frame.FromCvMat(mat, camera_format, timestamp);
  if (!s.ok()) return s;
  *this = {std::move(camera_frame), min, max};
  return Status::OK();
}
#endif

}  // namespace drivers
}  // namespace felicia