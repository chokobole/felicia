// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/slam/camera/distortion_matrix.h"

#include "third_party/chromium/base/strings/string_util.h"

namespace felicia {
namespace slam {

constexpr struct {
  DistortionModel distortion_model;
  const char* text;
} const kDistortionModelToText[] = {
    {DISTORTION_MODEL_EQUIDISTANT, "equidistant"},
    {DISTORTION_MODEL_EQUIDISTANT, "fisheye"},
    {DISTORTION_MODEL_PLUMB_BOB, "plumb_bob"},
    {DISTORTION_MODEL_RATIONAL_POLYNOMIAL, "rational_polynomial"},
};

std::string DistortionModelToString(DistortionModel distortion_model) {
  for (auto& item : kDistortionModelToText) {
    if (item.distortion_model == distortion_model) return item.text;
  }
  return base::EmptyString();
}

DistortionModel ToDistortionModel(const std::string& distortion_model) {
  for (auto& item : kDistortionModelToText) {
    if (item.text == distortion_model) return item.distortion_model;
  }
  return DISTORTION_MODEL_NONE;
}

}  // namespace slam
}  // namespace felicia