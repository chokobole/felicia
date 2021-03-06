// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_DRIVERS_IMU_IMU_FORMAT_H_
#define FELICIA_DRIVERS_IMU_IMU_FORMAT_H_

#include <vector>

#include "felicia/core/lib/base/export.h"
#include "felicia/drivers/imu/imu_format_message.pb.h"

namespace felicia {
namespace drivers {

class FEL_EXPORT ImuFormat {
 public:
  ImuFormat();
  ImuFormat(float frame_rate);

  ImuFormat(const ImuFormat& other);
  ImuFormat& operator=(const ImuFormat& other);

  std::string ToString() const;

  float frame_rate() const;
  void set_frame_rate(float frame_rate);

  ImuFormatMessage ToImuFormatMessage() const;

  bool operator==(const ImuFormat& other);

 private:
  float frame_rate_ = 0;
};

FEL_EXPORT std::ostream& operator<<(std::ostream& os,
                                    const ImuFormat& imu_format);

using ImuFormats = std::vector<ImuFormat>;

// Compares the priority of the imu formats. Returns true if |lhs| is the
// preferred imu format in comparison with |rhs|. Returns false otherwise.
FEL_EXPORT bool CompareCapability(const ImuFormat& requested,
                                  const ImuFormat& lhs, const ImuFormat& rhs);

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_IMU_IMU_FORMAT_H_