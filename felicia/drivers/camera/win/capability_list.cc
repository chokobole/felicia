// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/capability_list_win.cc

#include "felicia/drivers/camera/win/capability_list.h"

#include <algorithm>
#include <functional>

#include "third_party/chromium/base/logging.h"

namespace felicia {

namespace {

// Compares the priority of the capture formats. Returns true if |lhs| is the
// preferred capture format in comparison with |rhs|. Returns false otherwise.
bool CompareCapability(const CameraFormat& requested, const CameraFormat& lhs,
                       const CameraFormat& rhs) {
  const int diff_height_lhs = std::abs(lhs.height() - requested.height());
  const int diff_height_rhs = std::abs(rhs.height() - requested.height());
  if (diff_height_lhs != diff_height_rhs)
    return diff_height_lhs < diff_height_rhs;

  const int diff_width_lhs = std::abs(lhs.width() - requested.width());
  const int diff_width_rhs = std::abs(rhs.width() - requested.width());
  if (diff_width_lhs != diff_width_rhs) return diff_width_lhs < diff_width_rhs;

  const float diff_fps_lhs =
      std::fabs(lhs.frame_rate() - requested.frame_rate());
  const float diff_fps_rhs =
      std::fabs(rhs.frame_rate() - requested.frame_rate());
  if (diff_fps_lhs != diff_fps_rhs) return diff_fps_lhs < diff_fps_rhs;

  return false;
}

}  // namespace

const Capability* GetBestMatchedCapability(const CameraFormat& requested,
                                           const CapabilityList& capabilities) {
  DCHECK(!capabilities.empty());
  const Capability* best_match = nullptr;
  for (const Capability& capability : capabilities) {
    if (!best_match) {
      if (requested.pixel_format() ==
          capability.supported_format.pixel_format()) {
        best_match = &capability;
      }
      continue;
    } else {
      if (requested.pixel_format() ==
          capability.supported_format.pixel_format()) {
        if (CompareCapability(requested, capability.supported_format,
                              best_match->supported_format)) {
          best_match = &capability;
        }
      }
    }
  }
  return best_match;
}

}  // namespace felicia
