// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/drivers/imu/imu_filter_factory.h"

#include "third_party/chromium/base/memory/ptr_util.h"

#include "felicia/drivers/imu/complementary_filter/complementary_filter.h"
#include "felicia/drivers/imu/madgwick_filter/madgwick_filter.h"

namespace felicia {
namespace drivers {

std::unique_ptr<ImuFilterInterface> ImuFilterFactory::NewImuFilter(
    ImuFilterKind kind) {
  if (kind == COMPLEMENTARY_FILTER_KIND) {
    return base::WrapUnique(new ComplementaryFilter());
  } else if (kind == MADGWICK_FILTER_KIND) {
    return base::WrapUnique(new MadgwickFilter());
  }

  return nullptr;
}

}  // namespace drivers
}  // namespace felicia