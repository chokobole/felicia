// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/drivers/lidar/lidar_interface.h"

namespace felicia {
namespace drivers {

LidarInterface::LidarInterface(const LidarEndpoint& lidar_endpoint)
    : lidar_endpoint_(lidar_endpoint) {}

LidarInterface::~LidarInterface() = default;

bool LidarInterface::IsInitialized() const {
  return lidar_state_.IsInitialized();
}

bool LidarInterface::IsStarted() const { return lidar_state_.IsStarted(); }

bool LidarInterface::IsStopped() const { return lidar_state_.IsStopped(); }

}  // namespace drivers
}  // namespace felicia