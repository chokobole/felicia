// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/sink_input_pin_win.h

// Implement a DirectShow input pin used for receiving captured frames from
// a DirectShow Capture filter.

#ifndef FELICIA_DRIVERS_CAMERA_WIN_SINK_INPUT_PIN_H_
#define FELICIA_DRIVERS_CAMERA_WIN_SINK_INPUT_PIN_H_

#include "third_party/chromium/base/macros.h"

#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/camera/win/pin_base.h"
#include "felicia/drivers/camera/win/sink_filter.h"

namespace felicia {
namespace drivers {

// Const used for converting Seconds to REFERENCE_TIME.
extern const REFERENCE_TIME kSecondsToReferenceTime;

// Input pin of the SinkFilter.
class SinkInputPin : public PinBase {
 public:
  SinkInputPin(IBaseFilter* filter, SinkFilterObserver* observer);

  void SetRequestedMediaFormat(PixelFormat pixel_format, float frame_rate,
                               const BITMAPINFOHEADER& info_header);

  // Implement PinBase.
  bool IsMediaTypeValid(const AM_MEDIA_TYPE* media_type) override;
  bool GetValidMediaType(int index, AM_MEDIA_TYPE* media_type) override;

  STDMETHOD(Receive)(IMediaSample* media_sample) override;

 private:
  ~SinkInputPin() override;

  PixelFormat requested_pixel_format_;
  float requested_frame_rate_;
  BITMAPINFOHEADER requested_info_header_;
  CameraFormat resulting_format_;
  SinkFilterObserver* observer_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(SinkInputPin);
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_SINK_INPUT_PIN_H_
