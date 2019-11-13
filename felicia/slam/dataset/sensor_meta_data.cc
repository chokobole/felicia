// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/slam/dataset/sensor_meta_data.h"

namespace felicia {
namespace slam {

SensorMetaData::SensorMetaData() = default;

SensorMetaData::SensorMetaData(const SensorMetaData& other) = default;

SensorMetaData::SensorMetaData(SensorMetaData&& other) noexcept
    : left_K_(std::move(other.left_K_)),
      right_K_(std::move(other.right_K_)),
      left_D_(std::move(other.left_D_)),
      right_D_(std::move(other.right_D_)),
      left_P_(std::move(other.left_P_)),
      right_P_(std::move(other.right_P_)) {}

SensorMetaData::~SensorMetaData() = default;

SensorMetaData& SensorMetaData::operator=(const SensorMetaData& other) =
    default;
SensorMetaData& SensorMetaData::operator=(SensorMetaData&& other) = default;

#define DEFINE_METHOD(Type, name)                                         \
  bool SensorMetaData::has_##name() const { return name##_.has_value(); } \
  void SensorMetaData::set_##name(const Type& name) { name##_ = name; }   \
  const Type& SensorMetaData::name() const& { return name##_.value(); }   \
  Type&& SensorMetaData::name()&& { return std::move(name##_).value(); }

DEFINE_METHOD(EigenCameraMatrixd, left_K)
DEFINE_METHOD(EigenCameraMatrixd, right_K)
DEFINE_METHOD(EigenDistortionMatrixd, left_D)
DEFINE_METHOD(EigenDistortionMatrixd, right_D)
DEFINE_METHOD(EigenProjectionMatrixd, left_P)
DEFINE_METHOD(EigenProjectionMatrixd, right_P)

#undef DEFINE_METHOD

}  // namespace slam
}  // namespace felicia