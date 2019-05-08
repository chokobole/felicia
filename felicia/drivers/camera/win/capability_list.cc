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

const Capability& GetBestMatchedCapability(const CameraFormat& requested,
                                           const CapabilityList& capabilities) {
  DCHECK(!capabilities.empty());
  const Capability* best_match = &(*capabilities.begin());
  for (const Capability& capability : capabilities) {
    if (CompareCapability(requested, capability.supported_format,
                          best_match->supported_format)) {
      best_match = &capability;
    }
  }
  return *best_match;
}

}  // namespace felicia
