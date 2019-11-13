// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_DRIVERS_CAMERA_WIN_CAMERA_UTIL_H_
#define FELICIA_DRIVERS_CAMERA_WIN_CAMERA_UTIL_H_

#include <string>

namespace felicia {
namespace drivers {

std::string GetDeviceModelId(const std::string& device_id);

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_CAMERA_UTIL_H_