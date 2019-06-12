#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_

#include <memory>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/optional.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/lib/unit/geometry/size.h"
#include "felicia/core/message/message_io.h"
#include "felicia/drivers/camera/camera_buffer.h"
#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/camera/camera_frame_message.pb.h"

namespace felicia {

class EXPORT CameraFrame {
 public:
  CameraFrame(std::unique_ptr<uint8_t[]> data, size_t length,
              CameraFormat camera_format);
  CameraFrame(CameraFrame&& other) noexcept;
  CameraFrame& operator=(CameraFrame&& other);
  ~CameraFrame();

  std::unique_ptr<uint8_t[]> data();
  const uint8_t* data_ptr() const;
  int width() const;
  int height() const;
  size_t AllocationSize() const;
  PixelFormat pixel_format() const;

  void set_timestamp(::base::TimeDelta time);
  ::base::TimeDelta timestamp() const;

  CameraFrameMessage ToCameraFrameMessage() const;

 protected:
  std::unique_ptr<uint8_t[]> data_;
  size_t length_;
  CameraFormat camera_format_;
  ::base::TimeDelta timestamp_;

  DISALLOW_COPY_AND_ASSIGN(CameraFrame);
};

template <>
class DeflateTraits<CameraFrameMessage> {
 public:
  static bool ShouldDeflate(const CameraFrameMessage* proto,
                            const std::string& serialized) {
    return proto->camera_format().pixel_format() !=
           PixelFormat::PIXEL_FORMAT_MJPEG;
  }
};

EXPORT ::base::Optional<CameraFrame> ConvertToARGB(CameraBuffer camera_buffer,
                                                   CameraFormat camera_format);

typedef ::base::RepeatingCallback<void(CameraFrame)> CameraFrameCallback;

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_