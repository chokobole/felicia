#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_

#include <memory>

#if defined(HAS_OPENCV)
#include <opencv2/core.hpp>
#endif

#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/optional.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/containers/data.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/core/lib/image/image.h"
#include "felicia/core/lib/unit/geometry/size.h"
#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/camera/camera_frame_message.pb.h"

namespace felicia {
namespace drivers {

class EXPORT CameraFrame {
 public:
  CameraFrame();
  CameraFrame(const Data& data, const CameraFormat& camera_format,
              base::TimeDelta timestamp);
  CameraFrame(Data&& data, const CameraFormat& camera_format,
              base::TimeDelta timestamp) noexcept;
  CameraFrame(const Image& image, float frame_rate, base::TimeDelta timestamp);
  CameraFrame(Image&& image, float frame_rate,
              base::TimeDelta timestamp) noexcept;
  CameraFrame(const CameraFrame& other);
  CameraFrame(CameraFrame&& other) noexcept;
  CameraFrame& operator=(const CameraFrame& other);
  CameraFrame& operator=(CameraFrame&& other);
  ~CameraFrame();

  const Data& data() const;
  Data& data();
  size_t length() const;
  const CameraFormat& camera_format() const;
  int width() const;
  int height() const;
  float frame_rate() const;
  PixelFormat pixel_format() const;

  void set_timestamp(base::TimeDelta time);
  base::TimeDelta timestamp() const;

  CameraFrameMessage ToCameraFrameMessage(bool copy = true);
  Status FromCameraFrameMessage(const CameraFrameMessage& message);
  Status FromCameraFrameMessage(CameraFrameMessage&& message);

#if defined(HAS_OPENCV)
  // Copy to |out| if |copy| is true, Otherwise |out| references to |data_|.
  // Return true if it is success.
  bool ToCvMat(cv::Mat* out, bool copy = true);

  Status FromCvMat(cv::Mat mat, const CameraFormat& camera_format,
                   base::TimeDelta timestamp);
#endif

 protected:
  Data data_;
  CameraFormat camera_format_;
  base::TimeDelta timestamp_;
};

EXPORT base::Optional<CameraFrame> ConvertToRequestedPixelFormat(
    const uint8_t* data, size_t data_length, const CameraFormat& camera_format,
    PixelFormat requested_pixel_format, base::TimeDelta timestamp);

typedef base::RepeatingCallback<void(CameraFrame&&)> CameraFrameCallback;

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FRAME_H_