// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/drivers/camera/camera_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"
#include "third_party/chromium/build/build_config.h"

#if !defined(OS_MACOSX)
#include "felicia/core/lib/felicia_env.h"
#if defined(OS_LINUX)
#include "felicia/drivers/camera/linux/v4l2_camera.h"
using Camera = felicia::drivers::V4l2Camera;
#elif defined(OS_WIN)
#include "felicia/drivers/camera/win/dshow_camera.h"
#if !BUILDFLAG(TRAVIS)
#include "felicia/drivers/camera/win/mf_camera.h"
#else
using Camera = felicia::drivers::DshowCamera;
#endif  // !BUILDFLAG(TRAVIS)
#else
#error Not supported platform!
#endif

namespace felicia {
namespace drivers {

// static
std::unique_ptr<CameraInterface> CameraFactory::NewCamera(
    const CameraDescriptor& descriptor) {
#if defined(OS_WIN) && !BUILDFLAG(TRAVIS)
  if (MfCamera::PlatformSupportsMediaFoundation()) {
    return base::WrapUnique(new MfCamera(descriptor));
  } else {
    return base::WrapUnique(new DshowCamera(descriptor));
  }
#else
  return base::WrapUnique(new Camera(descriptor));
#endif
}

// static
Status CameraFactory::GetCameraDescriptors(
    CameraDescriptors* camera_descriptors) {
  DCHECK(camera_descriptors->empty());
#if defined(OS_WIN) && !BUILDFLAG(TRAVIS)
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
#if defined(OS_WIN) && !BUILDFLAG(TRAVIS)
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

}  // namespace drivers
}  // namespace felicia

#endif  // #if !defined(OS_MACOSX)