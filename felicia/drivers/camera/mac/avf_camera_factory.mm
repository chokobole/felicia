// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/drivers/camera/camera_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"

#include "felicia/drivers/camera/mac/avf_camera.h"

namespace felicia {
namespace drivers {

// static
std::unique_ptr<CameraInterface> CameraFactory::NewCamera(const CameraDescriptor& descriptor) {
  return base::WrapUnique(new AvfCamera(descriptor));
}

// static
Status CameraFactory::GetCameraDescriptors(CameraDescriptors* camera_descriptors) {
  DCHECK(camera_descriptors->empty());
  return AvfCamera::GetCameraDescriptors(camera_descriptors);
}

// static
Status CameraFactory::GetSupportedCameraFormats(const CameraDescriptor& camera_descriptor,
                                                CameraFormats* camera_formats) {
  DCHECK(camera_formats->empty());
  return AvfCamera::GetSupportedCameraFormats(camera_descriptor, camera_formats);
}

}  // namespace drivers
}  // namespace felicia