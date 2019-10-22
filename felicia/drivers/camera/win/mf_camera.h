// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/video_capture_device_mf_win.h

#ifndef FELICIA_DRIVERS_CAMERA_WIN_MF_CAMERA_H_
#define FELICIA_DRIVERS_CAMERA_WIN_MF_CAMERA_H_

#include "felicia/drivers/camera/camera_interface.h"

#include <mfcaptureengine.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <strmif.h>
#include <wrl/client.h>

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/sequence_checker.h"

#include "felicia/drivers/camera/win/capability_list.h"

namespace felicia {
namespace drivers {

class MFVideoCallback;

class MfCamera : public CameraInterface {
 public:
  ~MfCamera();

  // Needed by CameraFactory
  static bool PlatformSupportsMediaFoundation();
  static Status GetCameraDescriptors(CameraDescriptors* camera_descriptors);
  static Status GetSupportedCameraFormats(
      const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats);

  // CameraInterface methods
  Status Init() override;
  Status Start(const CameraFormat& requested_camera_format,
               CameraFrameCallback camera_frame_callback,
               StatusCallback status_callback) override;
  Status Stop() override;

  Status SetCameraSettings(const CameraSettings& camera_settings) override;
  Status GetCameraSettingsInfo(
      CameraSettingsInfoMessage* camera_settings) override;

  // Captured new video data.
  void OnIncomingCapturedData(const uint8_t* data, int length,
                              base::TimeTicks reference_time,
                              base::TimeDelta timestamp);
  void OnFrameDropped(Status s);
  void OnEvent(IMFMediaEvent* media_event);

  void set_max_retry_count_for_testing(int max_retry_count) {
    max_retry_count_ = max_retry_count;
  }

  void set_retry_delay_in_ms_for_testing(int retry_delay_in_ms) {
    retry_delay_in_ms_ = retry_delay_in_ms;
  }

 private:
  friend class CameraFactory;

  HRESULT ExecuteHresultCallbackWithRetries(
      base::RepeatingCallback<HRESULT()> callback);
  HRESULT GetDeviceStreamCount(IMFCaptureSource* source, DWORD* count);
  HRESULT GetDeviceStreamCategory(
      IMFCaptureSource* source, DWORD stream_index,
      MF_CAPTURE_ENGINE_STREAM_CATEGORY* stream_category);
  HRESULT GetAvailableDeviceMediaType(IMFCaptureSource* source,
                                      DWORD stream_index,
                                      DWORD media_type_index,
                                      IMFMediaType** type);

  HRESULT FillCapabilities(IMFCaptureSource* source, bool photo,
                           CapabilityList* capabilities);

  Status InitializeVideoAndCameraControls();
  void GetCameraSetting(long property, CameraSettingsModeValue* value,
                        bool camera_control = false);
  void GetCameraSetting(long property, CameraSettingsRangedValue* value,
                        bool camera_control = false);

  static Status GetCameraFormatFromSourceMediaType(
      IMFMediaType* source_media_type, bool photo, CameraFormat* camera_format);

  MfCamera(const CameraDescriptor& camera_descriptor);

  scoped_refptr<MFVideoCallback> video_callback_;
  int max_retry_count_;
  int retry_delay_in_ms_;

  // Guards the below variables from concurrent access between methods running
  // on |sequence_checker_| and calls to OnIncomingCapturedData() and OnEvent()
  // made by MediaFoundation on threads outside of our control.
  base::Lock lock_;

  Microsoft::WRL::ComPtr<IMFMediaSource> source_;
  Microsoft::WRL::ComPtr<IMFCaptureEngine> engine_;

  Microsoft::WRL::ComPtr<IAMCameraControl> camera_control_;
  Microsoft::WRL::ComPtr<IAMVideoProcAmp> video_control_;

  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_IMPLICIT_CONSTRUCTORS(MfCamera);
};

}  // namespace drivers
}  // namespace felicia

#endif  // FELICIA_DRIVERS_CAMERA_WIN_MF_CAMERA_H_