// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/base/win/mf_initializer.cc

#include "felicia/drivers/camera/win/mf_initializer.h"

#include <mfapi.h>

#include "third_party/chromium/base/logging.h"

namespace felicia {
namespace drivers {

bool InitializeMediaFoundation() {
  static const bool success = MFStartup(MF_VERSION, MFSTARTUP_LITE) == S_OK;
  DVLOG_IF(1, !success)
      << "Media Foundation unavailable or it failed to initialize";
  return success;
}

}  // namespace drivers
}  // namespace felicia
