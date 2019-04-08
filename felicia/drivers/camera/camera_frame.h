#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_

#include <memory>

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/optional.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/lib/unit/geometry/size.h"
#include "felicia/drivers/camera/camera_buffer.h"
#include "felicia/drivers/camera/camera_format.h"

namespace felicia {

class EXPORT CameraFrame {
 public:
  CameraFrame(uint8_t* data, CameraFormat camera_format);
  CameraFrame(const CameraFrame& other);
  CameraFrame& operator=(const CameraFrame& other);
  ~CameraFrame();

  uint8_t* data();
  size_t width() const;
  size_t height() const;
  size_t area() const;
  size_t size() const;
  CameraFormat::PixelFormat pixel_format() const;

  void set_timestamp(::base::Time time);
  ::base::Time timestamp() const;

 private:
  uint8_t* data_;
  CameraFormat camera_format_;
  ::base::Time timestamp_;
};

EXPORT ::base::Optional<CameraFrame> ConvertToARGB(CameraBuffer camera_buffer,
                                                   CameraFormat camera_format);

typedef ::base::RepeatingCallback<void(StatusOr<CameraFrame>)>
    CameraFrameCallback;

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_