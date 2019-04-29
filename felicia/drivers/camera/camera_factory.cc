#include "felicia/drivers/camera/camera_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/build/build_config.h"

#if defined(OS_LINUX)
#include "felicia/drivers/camera/linux/v4l2_camera.h"
using Camera = felicia::V4l2Camera;
#elif defined(OS_WIN)
#include "felicia/drivers/camera/win/dshow_camera.h"
using Camera = felicia::DshowCamera;
#elif defined(OS_MACOSX)
#include "felicia/drivers/camera/mac/avf_camera.h"
using Camera = felicia::AvfCamera;
#else
#include "felicia/drivers/camera/fake_camera.h"
using Camera = felicia::FakeCamera;
#endif

namespace felicia {

// static
std::unique_ptr<CameraInterface> CameraFactory::NewCamera(
    const CameraDescriptor& descriptor) {
  return ::base::WrapUnique(new Camera(descriptor));
}

// static
Status CameraFactory::GetCameraDescriptors(
    CameraDescriptors* camera_descriptors) {
  return Camera::GetCameraDescriptors(camera_descriptors);
}

}  // namespace felicia