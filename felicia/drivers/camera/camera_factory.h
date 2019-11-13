// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_FACTORY_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_FACTORY_H_

#include <memory>

#include "third_party/chromium/base/macros.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/drivers/camera/camera_interface.h"

namespace felicia {
namespace drivers {

class FEL_EXPORT CameraFactory {
 public:
  static std::unique_ptr<CameraInterface> NewCamera(
      const CameraDescriptor& descriptor);

  static Status GetCameraDescriptors(CameraDescriptors* camera_descriptors);

  static Status GetSupportedCameraFormats(
      const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats);

  DISALLOW_COPY_AND_ASSIGN(CameraFactory);
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_FACTORY_H_