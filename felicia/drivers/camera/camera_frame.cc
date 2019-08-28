#include "felicia/drivers/camera/camera_frame.h"

#include "libyuv.h"
#include "third_party/chromium/base/logging.h"

#include "felicia/drivers/camera/camera_frame_util.h"

namespace felicia {
namespace drivers {

CameraFrame::CameraFrame() = default;

CameraFrame::CameraFrame(const StringVector& data,
                         const CameraFormat& camera_format,
                         base::TimeDelta timestamp)
    : data_(data), camera_format_(camera_format), timestamp_(timestamp) {}

CameraFrame::CameraFrame(StringVector&& data, const CameraFormat& camera_format,
                         base::TimeDelta timestamp) noexcept
    : data_(std::move(data)),
      camera_format_(camera_format),
      timestamp_(timestamp) {}

CameraFrame::CameraFrame(const std::string& data,
                         const CameraFormat& camera_format,
                         base::TimeDelta timestamp)
    : CameraFrame(StringVector{data}, camera_format, timestamp) {}

CameraFrame::CameraFrame(std::string&& data, const CameraFormat& camera_format,
                         base::TimeDelta timestamp) noexcept
    : CameraFrame(StringVector{std::move(data)}, camera_format, timestamp) {}

CameraFrame::CameraFrame(const Image& image, float frame_rate,
                         base::TimeDelta timestamp)
    : CameraFrame(image.data(),
                  CameraFormat{image.size(), image.pixel_format(), frame_rate},
                  timestamp) {}

CameraFrame::CameraFrame(Image&& image, float frame_rate,
                         base::TimeDelta timestamp) noexcept
    : CameraFrame(std::move(image.data()),
                  CameraFormat{image.size(), image.pixel_format(), frame_rate},
                  timestamp) {}

CameraFrame::CameraFrame(const CameraFrame& other)
    : data_(other.data_),
      camera_format_(other.camera_format_),
      timestamp_(other.timestamp_) {}

CameraFrame::CameraFrame(CameraFrame&& other) noexcept
    : data_(std::move(other.data_)),
      camera_format_(other.camera_format_),
      timestamp_(other.timestamp_) {}

CameraFrame& CameraFrame::operator=(const CameraFrame& other) = default;
CameraFrame& CameraFrame::operator=(CameraFrame&& other) = default;

CameraFrame::~CameraFrame() = default;

const StringVector& CameraFrame::data() const { return data_; }

StringVector& CameraFrame::data() { return data_; }

size_t CameraFrame::length() const { return data_.size(); }

const CameraFormat& CameraFrame::camera_format() const {
  return camera_format_;
}

int CameraFrame::width() const { return camera_format_.width(); }

int CameraFrame::height() const { return camera_format_.height(); }

float CameraFrame::frame_rate() const { return camera_format_.frame_rate(); }

PixelFormat CameraFrame::pixel_format() const {
  return camera_format_.pixel_format();
}

void CameraFrame::set_timestamp(base::TimeDelta timestamp) {
  timestamp_ = timestamp;
}

base::TimeDelta CameraFrame::timestamp() const { return timestamp_; }

CameraFrameMessage CameraFrame::ToCameraFrameMessage(bool copy) {
  CameraFrameMessage message;

  if (copy) {
    message.set_data(data_.data());
  } else {
    message.set_data(std::move(data_).data());
  }
  *message.mutable_camera_format() = camera_format_.ToCameraFormatMessage();
  message.set_timestamp(timestamp_.InMicroseconds());

  return message;
}

Status CameraFrame::FromCameraFrameMessage(const CameraFrameMessage& message) {
  CameraFormat camera_format;
  Status s = camera_format.FromCameraFormatMessage(message.camera_format());
  if (!s.ok()) return s;

  *this = CameraFrame{message.data(), camera_format,
                      base::TimeDelta::FromMicroseconds(message.timestamp())};
  return Status::OK();
}

Status CameraFrame::FromCameraFrameMessage(CameraFrameMessage&& message) {
  std::unique_ptr<std::string> data(message.release_data());

  CameraFormat camera_format;
  Status s = camera_format.FromCameraFormatMessage(message.camera_format());
  if (!s.ok()) return s;

  *this = CameraFrame{std::move(*data), camera_format,
                      base::TimeDelta::FromMicroseconds(message.timestamp())};
  return Status::OK();
}

#if defined(HAS_OPENCV)
namespace {

int ToCvType(const CameraFrame* camera_frame) {
  if (!camera_frame->camera_format().HasFixedSizedChannelPixelFormat())
    return -1;
  int channel =
      camera_frame->length() / (camera_frame->width() * camera_frame->height());
  return CV_MAKETYPE(CV_8U, channel);
}

}  // namespace

bool CameraFrame::ToCvMat(cv::Mat* out, bool copy) {
  int type = ToCvType(this);
  if (type == -1) return false;

  cv::Mat mat(camera_format_.height(), camera_format_.width(), type,
              data_.cast<void*>());
  if (copy) {
    *out = mat.clone();
  } else {
    *out = mat;
  }
  return true;
}

Status CameraFrame::FromCvMat(cv::Mat mat, const CameraFormat& camera_format,
                              base::TimeDelta timestamp) {
  size_t length = mat.total() * mat.elemSize();
  *this = CameraFrame{StringVector{mat.data, length}, camera_format, timestamp};
  return Status::OK();
}
#endif

namespace {

base::Optional<CameraFrame> ConvertToBGRA(const uint8_t* data,
                                          size_t data_length,
                                          const CameraFormat& camera_format,
                                          base::TimeDelta timestamp) {
  PixelFormat pixel_format = camera_format.pixel_format();
  libyuv::FourCC src_format;

  if (pixel_format == PIXEL_FORMAT_BGRA) {
    LOG(ERROR) << "Its format is already PIXEL_FORMAT_BGRA.";
  }

  src_format = camera_format.ToLibyuvPixelFormat();
  if (src_format == libyuv::FOURCC_ANY) return base::nullopt;

  const int width = camera_format.width();
  const int height = camera_format.height();
  CameraFormat bgra_camera_format(width, height, PIXEL_FORMAT_BGRA,
                                  camera_format.frame_rate());
  size_t length = bgra_camera_format.AllocationSize();
  StringVector tmp_bgra;
  tmp_bgra.resize(length);
  uint8_t* tmp_bgra_ptr = tmp_bgra.cast<uint8_t*>();
  if (libyuv::ConvertToARGB(data, data_length, tmp_bgra_ptr, width * 4,
                            0 /* crop_x_pos */, 0 /* crop_y_pos */, width,
                            height, width, height,
                            libyuv::RotationMode::kRotate0, src_format) != 0) {
    return base::nullopt;
  }

  return CameraFrame(std::move(tmp_bgra), bgra_camera_format, timestamp);
}

}  // namespace

base::Optional<CameraFrame> ConvertToRequestedPixelFormat(
    const uint8_t* data, size_t data_length, const CameraFormat& camera_format,
    PixelFormat requested_pixel_format, base::TimeDelta timestamp) {
  if (requested_pixel_format == PIXEL_FORMAT_MJPEG) {
    return base::nullopt;
  } else if (requested_pixel_format == PIXEL_FORMAT_BGRA) {
    return ConvertToBGRA(data, data_length, camera_format, timestamp);
  } else {
    auto bgra_camera_frame_opt =
        ConvertToBGRA(data, data_length, camera_format, timestamp);
    if (!bgra_camera_frame_opt.has_value()) return base::nullopt;

    CameraFrame bgra_camera_frame = std::move(bgra_camera_frame_opt.value());
    const uint8_t* bgra_data = bgra_camera_frame.data().cast<const uint8_t*>();
    int width = bgra_camera_frame.width();
    int height = bgra_camera_frame.height();
    CameraFormat camera_format = bgra_camera_frame.camera_format();
    camera_format.set_pixel_format(requested_pixel_format);
    size_t length = camera_format.AllocationSize();
    StringVector tmp_camera_frame;
    tmp_camera_frame.resize(length);
    uint8_t* tmp_camera_frame_ptr = tmp_camera_frame.cast<uint8_t*>();

    int ret = -1;
    switch (requested_pixel_format) {
      case PIXEL_FORMAT_I420:
        ret = libyuv::ARGBToI420(
            bgra_data, width * 4, tmp_camera_frame_ptr, width,
            tmp_camera_frame_ptr + (width * height), width / 2,
            tmp_camera_frame_ptr + (width * height) * 5 / 4, width / 2, width,
            height);
        break;
      case PIXEL_FORMAT_YV12:
        // No conversion in libyuv api.
        break;
      case PIXEL_FORMAT_NV12:
        ret = libyuv::ARGBToNV12(bgra_data, width * 4, tmp_camera_frame_ptr,
                                 width, tmp_camera_frame_ptr + (width * height),
                                 width, width, height);
        break;
      case PIXEL_FORMAT_NV21:
        ret = libyuv::ARGBToNV21(bgra_data, width * 4, tmp_camera_frame_ptr,
                                 width, tmp_camera_frame_ptr + (width * height),
                                 width, width, height);
        break;
      case PIXEL_FORMAT_UYVY:
        ret = libyuv::ARGBToUYVY(bgra_data, width * 4, tmp_camera_frame_ptr,
                                 width * 2, width, height);
        break;
      case PIXEL_FORMAT_YUY2:
        ret = libyuv::ARGBToYUY2(bgra_data, width * 4, tmp_camera_frame_ptr,
                                 width * 2, width, height);
        break;
      case PIXEL_FORMAT_BGR:
        ret = libyuv::ARGBToRGB24(bgra_data, width * 4, tmp_camera_frame_ptr,
                                  width * 3, width, height);
        break;
      case PIXEL_FORMAT_RGBA:
        ret = libyuv::ARGBToABGR(bgra_data, width * 4, tmp_camera_frame_ptr,
                                 width * 4, width, height);
        break;
      case PIXEL_FORMAT_RGB:
        ret = libyuv::ARGBToRAW(bgra_data, width * 4, tmp_camera_frame_ptr,
                                width * 3, width, height);
        break;
      case PIXEL_FORMAT_ARGB:
        ret = libyuv::ARGBToBGRA(bgra_data, width * 4, tmp_camera_frame_ptr,
                                 width * 4, width, height);
        break;
      default:
        break;
    }
    if (ret != 0) return base::nullopt;

    return CameraFrame(std::move(tmp_camera_frame), camera_format, timestamp);
  }
}

}  // namespace drivers
}  // namespace felicia