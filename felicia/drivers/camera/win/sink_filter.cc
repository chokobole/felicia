// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)

#include "felicia/drivers/camera/win/sink_filter.h"

#include "third_party/chromium/base/logging.h"

#include "felicia/drivers/camera/win/sink_input_pin.h"

namespace felicia {

SinkFilterObserver::~SinkFilterObserver() {}

SinkFilter::SinkFilter(SinkFilterObserver* observer) : input_pin_(NULL) {
  input_pin_ = new SinkInputPin(this, observer);
}

void SinkFilter::SetRequestedMediaFormat(CameraFormat::PixelFormat pixel_format,
                                         float frame_rate,
                                         const BITMAPINFOHEADER& info_header) {
  input_pin_->SetRequestedMediaFormat(pixel_format, frame_rate, info_header);
}

size_t SinkFilter::NoOfPins() { return 1; }

IPin* SinkFilter::GetPin(int index) {
  return index == 0 ? input_pin_.get() : NULL;
}

STDMETHODIMP SinkFilter::GetClassID(CLSID* clsid) {
  *clsid = __uuidof(SinkFilter);
  return S_OK;
}

SinkFilter::~SinkFilter() { input_pin_->SetOwner(NULL); }

}  // namespace felicia
