#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FACTORY_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FACTORY_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {

class EXPORT CameraFactory {
 public:
  static std::unique_ptr<CameraInterface> NewCamera(
      const CameraDescriptor& descriptor);

  static Status GetCameraDescriptors(CameraDescriptors* camera_descriptors);

  DISALLOW_COPY_AND_ASSIGN(CameraFactory);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FACTORY_H_