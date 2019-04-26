// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)

#ifndef FELICIA_DRIVERS_CAEMRA_WIN_DSHOW_CAMERA_H_
#define FELICIA_DRIVERS_CAEMRA_WIN_DSHOW_CAMERA_H_

// Avoid including strsafe.h via dshow as it will cause build warnings.
#define NO_DSHOW_STRSAFE
#include <dshow.h>
#include <wrl/client.h>

#include "third_party/chromium/base/win/scoped_com_initializer.h"

#include "felicia/drivers/camera/camera_descriptor.h"
#include "felicia/drivers/camera/camera_interface.h"
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

  // CameraInterface methods
  Status Init() override;
  Status Start(CameraFrameCallback camera_frame_callback,
               StatusCallback status_callback) override;
  Status Close() override;

  StatusOr<CameraFormat> GetFormat() override;
  Status SetFormat(const CameraFormat& format) override;

  // SinkFilterObserver methods
  void FrameReceived(const uint8_t* buffer, int length,
                     const CameraFormat& format,
                     ::base::TimeDelta timestamp) override;

  void FrameDropped(const Status& s) override;

 private:
  static HRESULT GetDeviceFilter(const std::string& device_id,
                                 IBaseFilter** filter);

  static Microsoft::WRL::ComPtr<IPin> GetPin(IBaseFilter* filter,
                                             PIN_DIRECTION pin_dir,
                                             REFGUID category,
                                             REFGUID major_type);

  friend class CameraFactory;

  DshowCamera(const CameraDescriptor& descriptor);

  Microsoft::WRL::ComPtr<IBaseFilter> capture_filter_;

  Microsoft::WRL::ComPtr<IGraphBuilder> graph_builder_;
  Microsoft::WRL::ComPtr<ICaptureGraphBuilder2> capture_graph_builder_;

  Microsoft::WRL::ComPtr<IMediaControl> media_control_;
  Microsoft::WRL::ComPtr<IPin> input_sink_pin_;
  Microsoft::WRL::ComPtr<IPin> output_capture_pin_;

  scoped_refptr<SinkFilter> sink_filter_;

  CameraDescriptor descriptor_;
  CameraFormat camera_format_;

  ::base::TimeTicks first_ref_time_;

  ::base::win::ScopedCOMInitializer scoped_com_initializer_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(DshowCamera);
};

}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAEMRA_WIN_DSHOW_CAMERA_H_