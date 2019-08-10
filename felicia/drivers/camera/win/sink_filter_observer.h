// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/sink_filter_observer_win.h

// Observer class of Sinkfilter. The implementor of this class receive video
// frames from the SinkFilter DirectShow filter.

#ifndef FELICIA_DRIVERS_CAMERA_WIN_SINK_FILTER_OBSERVER_H_
#define FELICIA_DRIVERS_CAMERA_WIN_SINK_FILTER_OBSERVER_H_

#include <stdint.h>

#include "third_party/chromium/base/time/time.h"

#include "felicia/core/lib/error/status.h"
#include "felicia/drivers/camera/camera_format.h"

namespace felicia {
namespace drivers {

class SinkFilterObserver {
 public:
  // SinkFilter will call this function with all frames delivered to it.
  // |buffer| is only valid during this function call.
  virtual void FrameReceived(const uint8_t* buffer, int length,
                             const CameraFormat& format,
                             base::TimeDelta timestamp) = 0;

  virtual void FrameDropped(const Status& s) = 0;

 protected:
  virtual ~SinkFilterObserver();
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_SINK_FILTER_OBSERVER_H_
