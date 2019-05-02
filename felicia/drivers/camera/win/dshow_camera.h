// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/video_capture_device_win.h
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/video_capture_device_factory_win.h

#ifndef FELICIA_DRIVERS_CAEMRA_WIN_DSHOW_CAMERA_H_
#define FELICIA_DRIVERS_CAEMRA_WIN_DSHOW_CAMERA_H_

// Avoid including strsafe.h via dshow as it will cause build warnings.
#define NO_DSHOW_STRSAFE
#include <dshow.h>
#include <wrl/client.h>

#include "felicia/drivers/camera/camera_interface.h"
#include "felicia/drivers/camera/win/capability_list.h"
#include "felicia/drivers/camera/win/sink_filter.h"

namespace felicia {

class DshowCamera : public CameraInterface, SinkFilterObserver {
 public:
  // A utility class that wraps the AM_MEDIA_TYPE type and guarantees that
  // we free the structure when exiting the scope.  DCHECKing is also done to
  // avoid memory leaks.
  class ScopedMediaType {
   public:
    ScopedMediaType() : media_type_(NULL) {}
    ~ScopedMediaType() { Free(); }

    AM_MEDIA_TYPE* operator->() { return media_type_; }
    AM_MEDIA_TYPE* get() { return media_type_; }
    void Free();
    AM_MEDIA_TYPE** Receive();

   private:
    void FreeMediaType(AM_MEDIA_TYPE* mt);
    void DeleteMediaType(AM_MEDIA_TYPE* mt);

    AM_MEDIA_TYPE* media_type_;
  };

  ~DshowCamera();

  // Needed by CameraFactory
  static Status GetCameraDescriptors(CameraDescriptors* camera_descriptors);
  static Status GetSupportedCameraFormats(
      const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats);

  // CameraInterface methods
  Status Init() override;
  Status Start(CameraFrameCallback camera_frame_callback,
               StatusCallback status_callback) override;
  Status Stop() override;

  StatusOr<CameraFormat> GetCurrentCameraFormat() override;
  Status SetCameraFormat(const CameraFormat& format) override;

  // SinkFilterObserver methods
  void FrameReceived(const uint8_t* buffer, int length,
                     const CameraFormat& format,
                     ::base::TimeDelta timestamp) override;

  void FrameDropped(const Status& s) override;

 private:
  Status CreateCapabilityMap();

  static HRESULT EnumerateDirectShowDevices(IEnumMoniker** enum_moniker);
  static HRESULT GetDeviceFilter(const std::string& device_id,
                                 IBaseFilter** filter);

  static Microsoft::WRL::ComPtr<IPin> GetPin(IBaseFilter* filter,
                                             PIN_DIRECTION pin_dir,
                                             REFGUID category,
                                             REFGUID major_type);

  static void GetDeviceCapabilityList(const std::string& device_id,
                                      bool query_detailed_frame_rates,
                                      CapabilityList* out_capability_list);

  static void GetPinCapabilityList(
      Microsoft::WRL::ComPtr<IBaseFilter> capture_filter,
      Microsoft::WRL::ComPtr<IPin> output_capture_pin,
      bool query_detailed_frame_rates, CapabilityList* out_capability_list);

  friend class CameraFactory;

  DshowCamera(const CameraDescriptor& camera_descriptor);

  Microsoft::WRL::ComPtr<IBaseFilter> capture_filter_;

  Microsoft::WRL::ComPtr<IGraphBuilder> graph_builder_;
  Microsoft::WRL::ComPtr<ICaptureGraphBuilder2> capture_graph_builder_;

  Microsoft::WRL::ComPtr<IMediaControl> media_control_;
  Microsoft::WRL::ComPtr<IPin> input_sink_pin_;
  Microsoft::WRL::ComPtr<IPin> output_capture_pin_;

  scoped_refptr<SinkFilter> sink_filter_;

  // Map of all capabilities this device support.
  CapabilityList capabilities_;

  ::base::TimeTicks first_ref_time_;

  bool was_set_camera_format_ = false;

  DISALLOW_IMPLICIT_CONSTRUCTORS(DshowCamera);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAEMRA_WIN_DSHOW_CAMERA_H_