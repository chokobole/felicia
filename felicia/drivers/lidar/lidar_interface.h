// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_DRIVERS_LIDAR_LIDAR_INTERFACE_H_
#define FELICIA_DRIVERS_LIDAR_LIDAR_INTERFACE_H_

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/error/status.h"
#include "felicia/drivers/lidar/lidar_endpoint.h"
#include "felicia/drivers/lidar/lidar_frame.h"
#include "felicia/drivers/lidar/lidar_state.h"

namespace felicia {
namespace drivers {

class FEL_EXPORT LidarInterface {
 public:
  LidarInterface(const LidarEndpoint& lidar_endpoint);
  virtual ~LidarInterface();

  virtual Status Init() = 0;
  virtual Status Start(LidarFrameCallback lidar_frame_callback) = 0;
  virtual Status Stop() = 0;

  bool IsInitialized() const;
  bool IsStarted() const;
  bool IsStopped() const;

 protected:
  LidarEndpoint lidar_endpoint_;
  LidarState lidar_state_;

  LidarFrameCallback lidar_frame_callback_;
  StatusCallback status_callback_;
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_LIDAR_LIDAR_INTERFACE_H_