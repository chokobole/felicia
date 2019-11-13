// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_DRIVERS_CAMERA_CAMERA_ERRORS_H_
#define FELICIA_DRIVERS_CAMERA_CAMERA_ERRORS_H_

#include "third_party/chromium/base/strings/string_util.h"
#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/drivers/camera/camera_format.h"

namespace felicia {
namespace drivers {
namespace errors {

inline felicia::Status NoVideoCapbility() {
  return felicia::errors::NotFound("No video capability.");
}

inline felicia::Status FailedToGetCameraDescriptors() {
  return felicia::errors::Unavailable("Failed to get camera descriptors.");
}

inline felicia::Status FailedToGetFrameRate() {
  return felicia::errors::Unavailable("Failed to get frame rate.");
}

inline felicia::Status FailedToSetCameraFormat() {
  return felicia::errors::Unavailable("Failed to set camera format.");
}

inline felicia::Status FailedToSetFrameRate() {
  return felicia::errors::Unavailable("Failed to set frame rate.");
}

inline felicia::Status InvalidNumberOfBytesInBuffer() {
  return felicia::errors::InvalidArgument("Invalid number of bytes in buffer.");
}

inline felicia::Status FailedToConvertToRequestedPixelFormat(
    PixelFormat requested_pixel_format) {
  return felicia::errors::Unavailable(
      base::StringPrintf("Failed To convert to %s.",
                         PixelFormat_Name(requested_pixel_format).c_str()));
}

}  // namespace errors
}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_CAMERA_ERRORS_H_