#include "felicia/drivers/camera/camera_frame.h"

#include "libyuv.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/drivers/camera/camera_frame_util.h"

namespace felicia {

CameraFrame::CameraFrame(uint8_t* data, CameraFormat camera_format)
    : data_(std::move(data)), camera_format_(camera_format) {}

CameraFrame::CameraFrame(const CameraFrame& other) = default;
CameraFrame& CameraFrame::operator=(const CameraFrame& other) = default;

CameraFrame::~CameraFrame() = default;

uint8_t* CameraFrame::data() { return data_; }

size_t CameraFrame::width() const { return camera_format_.width(); }

size_t CameraFrame::height() const { return camera_format_.height(); }

size_t CameraFrame::size() const { return AllocationSize(camera_format_); }

CameraFormat::PixelFormat CameraFrame::pixel_format() const {
  return camera_format_.pixel_format();
}

::base::Optional<CameraFrame> ConvertToARGB(CameraBuffer camera_buffer,
                                            CameraFormat camera_format) {
  CameraFormat::PixelFormat pixel_format = camera_format.pixel_format();
  uint32_t src_format;

  if (pixel_format == CameraFormat::PIXEL_FORMAT_MJPEG) {
    NOTIMPLEMENTED();
  }

  if (pixel_format == CameraFormat::PIXEL_FORMAT_UYVY)
    src_format = libyuv::FOURCC_UYVY;
  else if (pixel_format == CameraFormat::PIXEL_FORMAT_YUY2)
    src_format = libyuv::FOURCC_YUY2;
  else if (pixel_format == CameraFormat::PIXEL_FORMAT_I420)
    src_format = libyuv::FOURCC_I420;
  else if (pixel_format == CameraFormat::PIXEL_FORMAT_RGB24)
    src_format = libyuv::FOURCC_24BG;
  else
    return ::base::nullopt;

  CameraFormat rgba_camera_format(camera_format.width(), camera_format.height(),
                                  CameraFormat::PIXEL_FORMAT_ARGB);
  // TODO(chokobole): Allocate this by std::unique_ptr, currently because of
  // StatusOr<CameraFrame> is not supported when CameraFrame is not copy
  // constructible.
  uint8_t* tmp_argb = new uint8_t[AllocationSize(rgba_camera_format)];
  if (libyuv::ConvertToARGB(camera_buffer.start(), camera_buffer.payload(),
                            tmp_argb, camera_format.width() * 4,
                            0 /* crop_x_pos */, 0 /* crop_y_pos */,
                            camera_format.width(), camera_format.height(),
                            camera_format.width(), camera_format.height(),
                            libyuv::RotationMode::kRotate0, src_format) != 0) {
    return ::base::nullopt;
  }

  return CameraFrame(tmp_argb, rgba_camera_format);
}

}  // namespace felicia