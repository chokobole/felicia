#ifndef FELICIA_DRIVERS_CAMERA_MAC_FRAME_RECEIVER_H_
#define FELICIA_DRIVERS_CAMERA_MAC_FRAME_RECEIVER_H_

#include "third_party/chromium/base/location.h"
#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/drivers/camera/camera_format.h"

namespace felicia {
namespace drivers {

class FrameReceiver {
 public:
  virtual ~FrameReceiver() = default;

  // Called to deliver captured video frames.  It's safe to call this method
  // from any thread, including those controlled by AVFoundation.
  virtual void ReceiveFrame(const uint8_t* video_frame, int video_frame_length,
                            const CameraFormat& camera_format,
                            int aspect_numerator, int aspect_denominator,
                            base::TimeDelta timestamp) = 0;

  // Forwarder to CameraInterface::status_callback_.
  virtual void ReceiveError(const Status& status) = 0;
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_MAC_FRAME_RECEIVER_H_