#ifndef FELICIA_DRIVERS_CAMERA_LINUX_V4L2_CAMERA_H_
#define FELICIA_DRIVERS_CAMERA_LINUX_V4L2_CAMERA_H_

#include <linux/videodev2.h>

#include "third_party/chromium/base/files/platform_file.h"
#include "third_party/chromium/base/memory/weak_ptr.h"
#include "third_party/chromium/base/threading/thread.h"

#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/drivers/camera/camera_interface.h"
#include "felicia/drivers/camera/linux/camera_buffer.h"

namespace felicia {

class V4l2Camera : public CameraInterface,
                   public ::base::SupportsWeakPtr<V4l2Camera> {
 public:
  ~V4l2Camera();

  Status Init() override;
  Status Start() override;
  Status Close() override;

  Status TakePhoto() override;

 private:
  friend class CameraFactory;

  V4l2Camera(const CameraDescriptor& descriptor);

  Status InitDevice();
  Status InitMmap();
  void DoTakePhoto();

  int DoIoctl(int request, void* argp);

  CameraDescriptor descriptor_;
  int fd_ = ::base::kInvalidPlatformFile;

  std::vector<CameraBuffer> buffers_;
  ::base::Thread thread_;

  Status error_status_;

  DISALLOW_COPY_AND_ASSIGN(V4l2Camera);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_LINUX_V4L2_CAMERA_H_