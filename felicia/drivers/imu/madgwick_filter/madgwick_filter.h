// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_DRIVERS_IMU_MADGWICK_FILTER_MADGWICK_FILTER_H_
#define FELICIA_DRIVERS_IMU_MADGWICK_FILTER_MADGWICK_FILTER_H_

#include "felicia/drivers/imu/imu_filter_interface.h"

namespace felicia {
namespace drivers {

class MadgwickFilter : public ImuFilterInterface {
 public:
  Quaternionf orientation() const override;
  void UpdateAngularVelocity(const Vector3f& angular_velocity,
                             base::TimeDelta timestamp) override;
  void UpdateLinearAcceleration(const Vector3f& linear_acceleration) override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 private:
  friend class ImuFilterFactory;

  MadgwickFilter();

  bool has_measurement_ = false;
  base::TimeDelta last_timestamp_;
  float beta_ = 0.1;
  Quaternionf gravity_;
  Eigen::Vector4f gradient_;
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_MADGWICK_FILTER_MADGWICK_FILTER_H_