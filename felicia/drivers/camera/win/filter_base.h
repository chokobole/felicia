// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/filter_base_win.h

// Implement a simple base class for DirectShow filters. It may only be used in
// a single threaded apartment.

#ifndef FELICIA_DRIVERS_CAMERA_WIN_FILTER_BASE_H_
#define FELICIA_DRIVERS_CAMERA_WIN_FILTER_BASE_H_

// Avoid including strsafe.h via dshow as it will cause build warnings.
#define NO_DSHOW_STRSAFE
#include <dshow.h>
#include <stddef.h>
#include <wrl/client.h>

#include "third_party/chromium/base/macros.h"
#include "third_party/chromium/base/memory/ref_counted.h"

namespace felicia {

class FilterBase : public IBaseFilter, public base::RefCounted<FilterBase> {
 public:
  FilterBase();

  // Number of pins connected to this filter.
  virtual size_t NoOfPins() = 0;
  // Returns the IPin interface pin no index.
  virtual IPin* GetPin(int index) = 0;

  // Inherited from IUnknown.
  STDMETHOD(QueryInterface)(REFIID id, void** object_ptr) override;
  STDMETHOD_(ULONG, AddRef)() override;
  STDMETHOD_(ULONG, Release)() override;

  // Inherited from IBaseFilter.
  STDMETHOD(EnumPins)(IEnumPins** enum_pins) override;

  STDMETHOD(FindPin)(LPCWSTR id, IPin** pin) override;

  STDMETHOD(QueryFilterInfo)(FILTER_INFO* info) override;

  STDMETHOD(JoinFilterGraph)(IFilterGraph* graph, LPCWSTR name) override;

  STDMETHOD(QueryVendorInfo)(LPWSTR* vendor_info) override;

  // Inherited from IMediaFilter.
  STDMETHOD(Stop)() override;

  STDMETHOD(Pause)() override;

  STDMETHOD(Run)(REFERENCE_TIME start) override;

  STDMETHOD(GetState)(DWORD msec_timeout, FILTER_STATE* state) override;

  STDMETHOD(SetSyncSource)(IReferenceClock* clock) override;

  STDMETHOD(GetSyncSource)(IReferenceClock** clock) override;

  // Inherited from IPersistent.
  STDMETHOD(GetClassID)(CLSID* class_id) override = 0;

 protected:
  friend class base::RefCounted<FilterBase>;
  virtual ~FilterBase();

 private:
  FILTER_STATE state_;
  Microsoft::WRL::ComPtr<IFilterGraph> owning_graph_;

  DISALLOW_COPY_AND_ASSIGN(FilterBase);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_FILTER_BASE_H_
