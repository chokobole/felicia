// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/capability_list_win.h

// Windows specific implementation of VideoCaptureDevice.
// DirectShow is used for capturing. DirectShow provide its own threads
// for capturing.

#ifndef FELICIA_DRIVERS_CAMERA_WIN_CAPABILITY_LIST_H_
#define FELICIA_DRIVERS_CAMERA_WIN_CAPABILITY_LIST_H_

#include <windows.h>
#include <list>

#include "felicia/drivers/camera/camera_format.h"

namespace felicia {

struct Capability {
  Capability(int media_type_index, const CameraFormat& format)
      : media_type_index(media_type_index),
        supported_format(format),
        info_header(),
        stream_index(0) {}

  // Used by DshowCamera.
  Capability(int media_type_index, const CameraFormat& format,
             const BITMAPINFOHEADER& info_header)
      : media_type_index(media_type_index),
        supported_format(format),
        info_header(info_header),
        stream_index(0) {}

  const int media_type_index;
  const CameraFormat supported_format;

  // |info_header| is only valid if DirectShow is used.
  const BITMAPINFOHEADER info_header;

  // |stream_index| is only valid if MediaFoundation is used.
  const int stream_index;
};

typedef std::list<Capability> CapabilityList;

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_CAPABILITY_LIST_H_
