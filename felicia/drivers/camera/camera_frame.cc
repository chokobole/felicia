#include "felicia/drivers/camera/camera_frame.h"

#include "libyuv.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/drivers/camera/camera_frame_util.h"

namespace felicia {

CameraFrame::CameraFrame(std::unique_ptr<uint8_t[]> data,
                         CameraFormat camera_format)
    : data_(std::move(data)), camera_format_(camera_format) {}

CameraFrame::CameraFrame(CameraFrame&& other) noexcept
    : data_(std::move(other.data_)),
      camera_format_(other.camera_format_),
      timestamp_(other.timestamp_) {}

CameraFrame& CameraFrame::operator=(CameraFrame&& other) {
  data_ = std::move(other.data_);
  camera_format_ = other.camera_format_;
  timestamp_ = other.timestamp_;

  return *this;
}

CameraFrame::~CameraFrame() = default;

std::unique_ptr<uint8_t[]> CameraFrame::data() { return std::move(data_); }

const uint8_t* CameraFrame::data_ptr() const { return data_.get(); }

int CameraFrame::width() const { return camera_format_.width(); }

int CameraFrame::height() const { return camera_format_.height(); }

size_t CameraFrame::AllocationSize() const {
  return camera_format_.AllocationSize();
}

PixelFormat CameraFrame::pixel_format() const {
  return camera_format_.pixel_format();
}

void CameraFrame::set_timestamp(::base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

::base::TimeDelta CameraFrame::timestamp() const { return timestamp_; }

CameraFrameMessage CameraFrame::ToCameraFrameMessage() const {
  CameraFrameMessage message;

  message.set_data(data_ptr(), AllocationSize());
  *message.mutable_camera_format() = camera_format_.ToCameraFormatMessage();
  message.set_timestamp(timestamp_.InMicroseconds());

  return message;
}

::base::Optional<CameraFrame> ConvertToARGB(CameraBuffer camera_buffer,
                                            CameraFormat camera_format) {
  PixelFormat pixel_format = camera_format.pixel_format();
  libyuv::FourCC src_format;

  if (pixel_format == PIXEL_FORMAT_MJPEG) {
    NOTIMPLEMENTED();
  }

  if (pixel_format == PIXEL_FORMAT_ARGB) {
    LOG(ERROR) << "Its format is already PIXEL_FORMAT_ARGB.";
  }

  src_format = camera_format.ToLibyuvPixelFormat();
  if (src_format == libyuv::FOURCC_ANY) return ::base::nullopt;

  CameraFormat rgba_camera_format(camera_format.width(), camera_format.height(),
                                  PIXEL_FORMAT_ARGB,
                                  camera_format.frame_rate());
  std::unique_ptr<uint8_t[]> tmp_argb(
      new uint8_t[rgba_camera_format.AllocationSize()]);
  if (libyuv::ConvertToARGB(camera_buffer.start(), camera_buffer.payload(),
                            tmp_argb.get(), camera_format.width() * 4,
                            0 /* crop_x_pos */, 0 /* crop_y_pos */,
                            camera_format.width(), camera_format.height(),
                            camera_format.width(), camera_format.height(),
                            libyuv::RotationMode::kRotate0, src_format) != 0) {
    return ::base::nullopt;
  }

  return CameraFrame(std::move(tmp_argb), rgba_camera_format);
}

}  // namespace felicia