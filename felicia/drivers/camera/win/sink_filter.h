// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/sink_filter_win.h

// Implement a DirectShow sink filter used for receiving captured frames from
// a DirectShow Capture filter.

#ifndef FELICIA_DRIVERS_CAMERA_WIN_SINK_FILTER_H_
#define FELICIA_DRIVERS_CAMERA_WIN_SINK_FILTER_H_

#include <stddef.h>
#include <windows.h>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/memory/ref_counted.h"

#include "felicia/drivers/camera/camera_format.h"
#include "felicia/drivers/camera/win/filter_base.h"
#include "felicia/drivers/camera/win/sink_filter_observer.h"

namespace felicia {

// Define GUID for I420. This is the color format we would like to support but
// it is not defined in the DirectShow SDK.
// http://msdn.microsoft.com/en-us/library/dd757532.aspx
// 30323449-0000-0010-8000-00AA00389B71.
const GUID kMediaSubTypeI420 = {
    0x30323449,
    0x0000,
    0x0010,
    {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};

// UYVY synonym with BT709 color components, used in HD video. This variation
// might appear in non-USB capture cards and it's implemented as a normal YUV
// pixel format with the characters HDYC encoded in the first array word.
const GUID kMediaSubTypeHDYC = {
    0x43594448,
    0x0000,
    0x0010,
    {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
;

// 16-bit grey-scale single plane formats provided by some depth cameras.
const GUID kMediaSubTypeZ16 = {
    0x2036315a,
    0x0000,
    0x0010,
    {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
const GUID kMediaSubTypeINVZ = {
    0x5a564e49,
    0x2d90,
    0x4a58,
    {0x92, 0x0b, 0x77, 0x3f, 0x1f, 0x2c, 0x55, 0x6b}};
const GUID kMediaSubTypeY16 = {
    0x20363159,
    0x0000,
    0x0010,
    {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};

class SinkInputPin;

class __declspec(uuid("88cdbbdc-a73b-4afa-acbf-15d5e2ce12c3")) SinkFilter
    : public FilterBase {
 public:
  explicit SinkFilter(SinkFilterObserver* observer);

  void SetRequestedMediaFormat(CameraFormat::PixelFormat pixel_format,
                               float frame_rate,
                               const BITMAPINFOHEADER& info_header);

  // Implement FilterBase.
  size_t NoOfPins() override;
  IPin* GetPin(int index) override;

  STDMETHOD(GetClassID)(CLSID* clsid) override;

 private:
  ~SinkFilter() override;

  scoped_refptr<SinkInputPin> input_pin_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(SinkFilter);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_SINK_FILTER_H_
