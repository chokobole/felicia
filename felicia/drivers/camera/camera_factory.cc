#include "felicia/drivers/camera/camera_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/build/build_config.h"

#if !defined(OS_MACOSX)
#if defined(OS_LINUX)
#include "felicia/drivers/camera/linux/v4l2_camera.h"
using Camera = felicia::V4l2Camera;
#elif defined(OS_WIN)
#include "felicia/drivers/camera/win/dshow_camera.h"
#include "felicia/drivers/camera/win/mf_camera.h"
#else
#include "felicia/drivers/camera/fake_camera.h"
using Camera = felicia::FakeCamera;
#endif

namespace felicia {

// static
std::unique_ptr<CameraInterface> CameraFactory::NewCamera(
    const CameraDescriptor& descriptor) {
#if defined(OS_WIN)
  if (MfCamera::PlatformSupportsMediaFoundation()) {
    return ::base::WrapUnique(new MfCamera(descriptor));
  } else {
    return ::base::WrapUnique(new DshowCamera(descriptor));
  }
#else
  return ::base::WrapUnique(new Camera(descriptor));
#endif
}

// static
Status CameraFactory::GetCameraDescriptors(
    CameraDescriptors* camera_descriptors) {
  DCHECK(camera_descriptors->empty());
#if defined(OS_WIN)
  if (MfCamera::PlatformSupportsMediaFoundation()) {
    return MfCamera::GetCameraDescriptors(camera_descriptors);
  } else {
    return DshowCamera::GetCameraDescriptors(camera_descriptors);
  }
#else
  return Camera::GetCameraDescriptors(camera_descriptors);
#endif
}

// static
Status CameraFactory::GetSupportedCameraFormats(
    const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats) {
  DCHECK(camera_formats->empty());
#if defined(OS_WIN)
  if (MfCamera::PlatformSupportsMediaFoundation()) {
    return MfCamera::GetSupportedCameraFormats(camera_descriptor,
                                               camera_formats);
  } else {
    return DshowCamera::GetSupportedCameraFormats(camera_descriptor,
                                                  camera_formats);
  }
#else
  return Camera::GetSupportedCameraFormats(camera_descriptor, camera_formats);
#endif
}

}  // namespace felicia

#endif  // #if !defined(OS_MACOSX)