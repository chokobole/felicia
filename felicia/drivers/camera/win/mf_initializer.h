// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/base/win/mf_initializer.h

#ifndef FELICIA_DRIVERS_CAMERA_WIN_MF_INITIALIZER_H_
#define FELICIA_DRIVERS_CAMERA_WIN_MF_INITIALIZER_H_

namespace felicia {
namespace drivers {

// Makes sure MFStartup() is called exactly once.  Returns true if Media
// Foundation is available and has been initialized successfully.  Note that it
// is expected to return false on an "N" edition of Windows, see
// https://en.wikipedia.org/wiki/Windows_7_editions#Special-purpose_editions.
bool InitializeMediaFoundation();

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_MF_INITIALIZER_H_
