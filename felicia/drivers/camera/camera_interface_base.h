#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_INTERFACE_BASE_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_INTERFACE_BASE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/statusor.h"
#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/camera/camera_frame.h"
#include "felicia/drivers/camera/camera_settings.h"
#include "felicia/drivers/camera/camera_state.h"

namespace felicia {
namespace drivers {

class FEL_EXPORT CameraInterfaceBase {
 public:
  CameraInterfaceBase(const CameraDescriptor& camera_descriptor);
  virtual ~CameraInterfaceBase();

  virtual Status Init() = 0;
  virtual Status Stop() = 0;

  virtual Status SetCameraSettings(const CameraSettings& camera_settings);
  virtual Status GetCameraSettingsInfo(
      CameraSettingsInfoMessage* camera_settings);

  bool IsInitialized() const;
  bool IsStarted() const;
  bool IsStopped() const;

 protected:
  CameraDescriptor camera_descriptor_;
  CameraState camera_state_;

  StatusCallback status_callback_;
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_INTERFACE_BASE_H_