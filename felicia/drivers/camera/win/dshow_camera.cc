// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/video_capture_device_win.cc
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/video_capture_device_factory_win.cc

#include "felicia/drivers/camera/win/dshow_camera.h"

#include <ksmedia.h>

#include "third_party/chromium/base/strings/sys_string_conversions.h"
#include "third_party/chromium/base/win/scoped_co_mem.h"
#include "third_party/chromium/base/win/scoped_variant.h"

#include "felicia/drivers/camera/camera_errors.h"
#include "felicia/drivers/camera/timestamp_constants.h"
#include "felicia/drivers/camera/win/camera_util.h"
#include "felicia/drivers/camera/win/sink_input_pin.h"

using base::win::ScopedCoMem;
using base::win::ScopedVariant;
using Microsoft::WRL::ComPtr;

namespace felicia {

#define MESSAGE_WITH_HRESULT(text, hr) \
  base::StringPrintf("%s :%s.", text,  \
                     ::logging::SystemErrorCodeToString(hr).c_str())

// Check if a Pin matches a category.
bool PinMatchesCategory(IPin* pin, REFGUID category) {
  DCHECK(pin);
  bool found = false;
  ComPtr<IKsPropertySet> ks_property;
  HRESULT hr = pin->QueryInterface(IID_PPV_ARGS(&ks_property));
  if (SUCCEEDED(hr)) {
    GUID pin_category;
    DWORD return_value;
    hr = ks_property->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0,
                          &pin_category, sizeof(pin_category), &return_value);
    if (SUCCEEDED(hr) && (return_value == sizeof(pin_category))) {
      found = (pin_category == category);
    }
  }
  return found;
}

// Check if a Pin's MediaType matches a given |major_type|.
bool PinMatchesMajorType(IPin* pin, REFGUID major_type) {
  DCHECK(pin);
  AM_MEDIA_TYPE connection_media_type;
  const HRESULT hr = pin->ConnectionMediaType(&connection_media_type);
  return SUCCEEDED(hr) && connection_media_type.majortype == major_type;
}

void DshowCamera::ScopedMediaType::Free() {
  if (!media_type_) return;

  DeleteMediaType(media_type_);
  media_type_ = NULL;
}

AM_MEDIA_TYPE** DshowCamera::ScopedMediaType::Receive() {
  DCHECK(!media_type_);
  return &media_type_;
}

// Release the format block for a media type.
// http://msdn.microsoft.com/en-us/library/dd375432(VS.85).aspx
void DshowCamera::ScopedMediaType::FreeMediaType(AM_MEDIA_TYPE* mt) {
  if (mt->cbFormat != 0) {
    CoTaskMemFree(mt->pbFormat);
    mt->cbFormat = 0;
    mt->pbFormat = NULL;
  }
  if (mt->pUnk != NULL) {
    NOTREACHED();
    // pUnk should not be used.
    mt->pUnk->Release();
    mt->pUnk = NULL;
  }
}

// Delete a media type structure that was allocated on the heap.
// http://msdn.microsoft.com/en-us/library/dd375432(VS.85).aspx
void DshowCamera::ScopedMediaType::DeleteMediaType(AM_MEDIA_TYPE* mt) {
  if (mt != NULL) {
    FreeMediaType(mt);
    CoTaskMemFree(mt);
  }
}

DshowCamera::DshowCamera(const CameraDescriptor& camera_descriptor)
    : CameraInterface(camera_descriptor) {}

DshowCamera::~DshowCamera() {
  if (media_control_.Get()) media_control_->Stop();

  if (graph_builder_.Get()) {
    if (sink_filter_.get()) {
      graph_builder_->RemoveFilter(sink_filter_.get());
      sink_filter_ = NULL;
    }

    if (capture_filter_.Get())
      graph_builder_->RemoveFilter(capture_filter_.Get());
  }

  if (capture_graph_builder_.Get()) capture_graph_builder_.Reset();
}

// static
Status DshowCamera::GetCameraDescriptors(
    CameraDescriptors* camera_descriptors) {
  DCHECK(camera_descriptors);

  ComPtr<IEnumMoniker> enum_moniker;
  HRESULT hr = EnumerateDirectShowDevices(enum_moniker.GetAddressOf());
  // CreateClassEnumerator returns S_FALSE on some Windows OS
  // when no camera exist. Therefore the FAILED macro can't be used.
  if (hr != S_OK) return Status::OK();

  // Enumerate all video capture devices.
  for (ComPtr<IMoniker> moniker;
       enum_moniker->Next(1, moniker.GetAddressOf(), NULL) == S_OK;
       moniker.Reset()) {
    ComPtr<IPropertyBag> prop_bag;
    hr = moniker->BindToStorage(0, 0, IID_PPV_ARGS(&prop_bag));
    if (FAILED(hr)) continue;

    // Find the description or friendly name.
    ScopedVariant name;
    hr = prop_bag->Read(L"Description", name.Receive(), 0);
    if (FAILED(hr)) hr = prop_bag->Read(L"FriendlyName", name.Receive(), 0);

    if (FAILED(hr) || name.type() != VT_BSTR) continue;

    const std::string device_name(base::SysWideToUTF8(V_BSTR(name.ptr())));

    name.Reset();
    hr = prop_bag->Read(L"DevicePath", name.Receive(), 0);
    std::string id;
    if (FAILED(hr) || name.type() != VT_BSTR) {
      id = device_name;
    } else {
      DCHECK_EQ(name.type(), VT_BSTR);
      id = base::SysWideToUTF8(V_BSTR(name.ptr()));
    }

    const std::string model_id = GetDeviceModelId(id);

    camera_descriptors->emplace_back(device_name, id, model_id);
  }

  return Status::OK();
}

// static
Status DshowCamera::GetSupportedCameraFormats(
    const CameraDescriptor& camera_descriptor, CameraFormats* camera_formats) {
  DCHECK(camera_formats->empty());

  CapabilityList capability_list;
  GetDeviceCapabilityList(camera_descriptor.device_id(), true,
                          &capability_list);
  for (const auto& entry : capability_list) {
    camera_formats->emplace_back(entry.supported_format);
    DVLOG(1) << camera_descriptor.display_name() << " "
             << entry.supported_format.ToString();
  }

  return Status::OK();
}

Status DshowCamera::Init() {
  if (!camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }

  HRESULT hr = GetDeviceFilter(camera_descriptor_.device_id(),
                               capture_filter_.GetAddressOf());
  if (FAILED(hr))
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to create capture filter", hr));

  output_capture_pin_ = GetPin(capture_filter_.Get(), PINDIR_OUTPUT,
                               PIN_CATEGORY_CAPTURE, GUID_NULL);
  if (!output_capture_pin_.Get())
    return errors::Unavailable("Failed to get capture output pin.");

  // Create the sink filter used for receiving Captured frames.
  sink_filter_ = new SinkFilter(this);
  if (sink_filter_.get() == NULL)
    return errors::Unavailable("Failed to create sink filter.");

  input_sink_pin_ = sink_filter_->GetPin(0);

  hr = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&graph_builder_));
  if (FAILED(hr))
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to create the Capture Graph Builder", hr));

  hr = ::CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
                          IID_PPV_ARGS(&capture_graph_builder_));
  if (FAILED(hr))
    return errors::Unavailable(MESSAGE_WITH_HRESULT(
        "Failed to give graph to capture graph builder", hr));

  hr = capture_graph_builder_->SetFiltergraph(graph_builder_.Get());
  if (FAILED(hr))
    return errors::Unavailable(MESSAGE_WITH_HRESULT(
        "Failed to give graph to capture graph builder", hr));

  hr = graph_builder_.CopyTo(media_control_.GetAddressOf());
  if (FAILED(hr))
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to create media control builder", hr));

  hr = graph_builder_->AddFilter(capture_filter_.Get(), NULL);
  if (FAILED(hr))
    return errors::Unavailable(MESSAGE_WITH_HRESULT(
        "Failed to add the capture device to the graph", hr));

  hr = graph_builder_->AddFilter(sink_filter_.get(), NULL);
  if (FAILED(hr))
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to add the sink filter to the graph", hr));

  camera_state_.ToInitialized();

  return Status::OK();
}

Status DshowCamera::Start(const CameraFormat& requested_camera_format,
                          CameraFrameCallback camera_frame_callback,
                          StatusCallback status_callback) {
  if (!camera_state_.IsInitialized()) {
    return camera_state_.InvalidStateError();
  }

  CapabilityList capabilities;
  GetPinCapabilityList(capture_filter_, output_capture_pin_,
                       true /* query_detailed_frame_rates */, &capabilities);
  if (capabilities.empty()) return errors::NoVideoCapbility();

  // Get the camera capability that best match the requested format.
  const Capability& found_capability =
      GetBestMatchedCapability(requested_camera_format, capabilities);

  // Reduce the frame rate if the requested frame rate is lower
  // than the capability.
  const float frame_rate =
      std::min(requested_camera_format.frame_rate(),
               found_capability.supported_format.frame_rate());

  ComPtr<IAMStreamConfig> stream_config;
  HRESULT hr = output_capture_pin_.CopyTo(stream_config.GetAddressOf());
  if (FAILED(hr)) {
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to get IAMStreamConfig", hr));
  }

  int count = 0, size = 0;
  hr = stream_config->GetNumberOfCapabilities(&count, &size);
  if (FAILED(hr)) {
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to GetNumberOfCapabilities", hr));
  }

  std::unique_ptr<BYTE[]> caps(new BYTE[size]);
  ScopedMediaType media_type;

  // Get the windows capability from the capture device.
  // GetStreamCaps can return S_FALSE which we consider an error. Therefore the
  // FAILED macro can't be used.
  hr = stream_config->GetStreamCaps(found_capability.media_type_index,
                                    media_type.Receive(), caps.get());
  if (hr != S_OK) {
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to GetStreamCaps", hr));
  }

  // Set the sink filter to request this format.
  sink_filter_->SetRequestedMediaFormat(
      found_capability.supported_format.pixel_format(), frame_rate,
      found_capability.info_header);
  // Order the capture device to use this format.
  hr = stream_config->SetFormat(media_type.get());
  if (hr != S_OK) {
    return errors::Unavailable(MESSAGE_WITH_HRESULT("Failed to SetFormat", hr));
  }

  requested_pixel_format_ = requested_camera_format.pixel_format();
  camera_format_ = found_capability.supported_format;
  camera_format_.set_frame_rate(frame_rate);

  if (media_type->subtype == kMediaSubTypeHDYC) {
    // HDYC pixel format, used by the DeckLink capture card, needs an AVI
    // decompressor filter after source, let |graph_builder_| add it.
    hr = graph_builder_->Connect(output_capture_pin_.Get(),
                                 input_sink_pin_.Get());
  } else {
    hr = graph_builder_->ConnectDirect(output_capture_pin_.Get(),
                                       input_sink_pin_.Get(), NULL);
  }

  if (FAILED(hr))
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to connect the CaptureGraph", hr));

  hr = media_control_->Pause();
  if (FAILED(hr))
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to pause the capture device", hr));

  // Start capturing.
  hr = media_control_->Run();
  if (FAILED(hr))
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to run the capture device", hr));

  camera_frame_callback_ = camera_frame_callback;
  status_callback_ = status_callback;

  camera_state_.ToStarted();

  return Status::OK();
}

Status DshowCamera::Stop() {
  if (!camera_state_.IsStarted()) {
    return camera_state_.InvalidStateError();
  }

  HRESULT hr = media_control_->Stop();
  if (FAILED(hr)) {
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to stop the capture device", hr));
  }

  graph_builder_->Disconnect(output_capture_pin_.Get());
  graph_builder_->Disconnect(input_sink_pin_.Get());

  camera_frame_callback_.Reset();
  status_callback_.Reset();
  camera_state_.ToStopped();

  return Status::OK();
}

Status DshowCamera::SetCameraSettings(const CameraSettings& camera_settings) {
  if (camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }

  if (!camera_control_ || !video_control_) {
    Status s = InitializeVideoAndCameraControls();
    if (!s.ok()) return s;
  }

  HRESULT hr;
  if (camera_settings.has_white_balance_mode()) {
    bool is_auto =
        camera_settings.white_balance_mode() == CAMERA_SETTINGS_MODE_AUTO;
    const long value = is_auto ? 1 : 0;
    const long flag =
        is_auto ? VideoProcAmp_Flags_Auto : VideoProcAmp_Flags_Manual;
    hr = video_control_->put_WhiteBalance(value, flag);
    if (FAILED(hr)) DLOG(ERROR) << "setting whilte_balance_mode to " << value;
  }

  if (camera_settings.has_color_temperature()) {
    bool can_set = false;
    {
      long value, flag;
      hr = video_control_->get_WhiteBalance(&value, &flag);
      can_set = SUCCEEDED(hr) && flag & VideoProcAmp_Flags_Manual;
    }
    if (can_set) {
      const long value = camera_settings.color_temperature();
      hr = video_control_->put_WhiteBalance(value, VideoProcAmp_Flags_Manual);
      if (FAILED(hr)) DLOG(ERROR) << "setting color_temperature to " << value;
    }
  }

  if (camera_settings.has_exposure_mode()) {
    bool is_auto = camera_settings.exposure_mode() == CAMERA_SETTINGS_MODE_AUTO;
    const long value = is_auto ? 1 : 0;
    const long flag =
        is_auto ? CameraControl_Flags_Auto : CameraControl_Flags_Manual;
    hr = camera_control_->put_Exposure(value, flag);
    if (FAILED(hr)) DLOG(ERROR) << "setting exposure_mode to " << value;
  }

  if (camera_settings.has_exposure_compensation()) {
    bool can_set = false;
    {
      long value, flag;
      hr = camera_control_->get_Exposure(&value, &flag);
      can_set = SUCCEEDED(hr) && flag & CameraControl_Flags_Manual;
    }
    if (can_set) {
      const long value = camera_settings.exposure_compensation();
      hr = camera_control_->put_Exposure(value, CameraControl_Flags_Manual);
      if (FAILED(hr))
        DLOG(ERROR) << "setting exposure_compensation to " << value;
    }
  }

  if (camera_settings.has_brightness()) {
    const long value = camera_settings.brightness();
    hr = video_control_->put_Brightness(value, VideoProcAmp_Flags_Manual);
    if (FAILED(hr)) DLOG(ERROR) << "setting brightness to " << value;
  }

  if (camera_settings.has_contrast()) {
    const long value = camera_settings.contrast();
    hr = video_control_->put_Contrast(value, VideoProcAmp_Flags_Manual);
    if (FAILED(hr)) DLOG(ERROR) << "setting contrast to " << value;
  }

  if (camera_settings.has_saturation()) {
    const long value = camera_settings.saturation();
    hr = video_control_->put_Saturation(value, VideoProcAmp_Flags_Manual);
    if (FAILED(hr)) DLOG(ERROR) << "setting saturation to " << value;
  }

  if (camera_settings.has_sharpness()) {
    const long value = camera_settings.sharpness();
    hr = video_control_->put_Sharpness(value, VideoProcAmp_Flags_Manual);
    if (FAILED(hr)) DLOG(ERROR) << "setting sharpness to " << value;
  }

  if (camera_settings.has_hue()) {
    const long value = camera_settings.hue();
    hr = video_control_->put_Hue(value, VideoProcAmp_Flags_Manual);
    if (FAILED(hr)) DLOG(ERROR) << "setting hue to " << value;
  }

  if (camera_settings.has_gain()) {
    const long value = camera_settings.gain();
    hr = video_control_->put_Gain(value, VideoProcAmp_Flags_Manual);
    if (FAILED(hr)) DLOG(ERROR) << "setting gain to " << value;
  }

  if (camera_settings.has_gamma()) {
    const long value = camera_settings.gamma();
    hr = video_control_->put_Gamma(value, VideoProcAmp_Flags_Manual);
    if (FAILED(hr)) DLOG(ERROR) << "setting gamma to " << value;
  }

  return Status::OK();
}

Status DshowCamera::GetCameraSettingsInfo(
    CameraSettingsInfoMessage* camera_settings) {
  if (camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }

  if (!camera_control_ || !video_control_) {
    Status s = InitializeVideoAndCameraControls();
    if (!s.ok()) return s;
  }

  GetCameraSetting(VideoProcAmp_WhiteBalance,
                   camera_settings->mutable_white_balance_mode());
  GetCameraSetting(CameraControl_Exposure,
                   camera_settings->mutable_exposure_mode(),
                   true /* camera_control */);
  GetCameraSetting(CameraControl_Exposure,
                   camera_settings->mutable_exposure_compensation(),
                   true /* camera_control */);
  GetCameraSetting(VideoProcAmp_WhiteBalance,
                   camera_settings->mutable_color_temperature());
  GetCameraSetting(VideoProcAmp_Brightness,
                   camera_settings->mutable_brightness());
  GetCameraSetting(VideoProcAmp_Contrast, camera_settings->mutable_contrast());
  GetCameraSetting(VideoProcAmp_Saturation,
                   camera_settings->mutable_saturation());
  GetCameraSetting(VideoProcAmp_Sharpness,
                   camera_settings->mutable_sharpness());
  GetCameraSetting(VideoProcAmp_Hue, camera_settings->mutable_hue());
  GetCameraSetting(VideoProcAmp_Gain, camera_settings->mutable_gain());
  GetCameraSetting(VideoProcAmp_Gamma, camera_settings->mutable_gamma());

  return Status::OK();
}

void DshowCamera::FrameReceived(const uint8_t* buffer, int length,
                                const CameraFormat& camera_format,
                                base::TimeDelta timestamp) {
  if (camera_format_.pixel_format() != PixelFormat::PIXEL_FORMAT_MJPEG &&
      camera_format_.AllocationSize() != length) {
    status_callback_.Run(errors::InvalidNumberOfBytesInBuffer());
    return;
  }

  // There is a chance that the platform does not provide us with the
  // timestamp, in which case, we use reference time to calculate a
  // timestamp.
  if (timestamp == kNoTimestamp) timestamp = timestamper_.timestamp();

  if (requested_pixel_format_ == camera_format_.pixel_format()) {
    std::unique_ptr<uint8_t[]> data(new uint8_t[length]);
    memcpy(data.get(), buffer, length);
    camera_frame_callback_.Run(CameraFrame{std::move(data),
                                           static_cast<size_t>(length),
                                           camera_format_, timestamp});
  } else {
    base::Optional<CameraFrame> camera_frame = ConvertToRequestedPixelFormat(
        buffer, length, camera_format_, requested_pixel_format_, timestamp);
    if (camera_frame.has_value()) {
      camera_frame_callback_.Run(std::move(camera_frame.value()));
    } else {
      status_callback_.Run(errors::FailedToConvertToRequestedPixelFormat(
          requested_pixel_format_));
    }
  }
}

void DshowCamera::FrameDropped(const Status& s) { status_callback_.Run(s); }

// static
HRESULT DshowCamera::EnumerateDirectShowDevices(IEnumMoniker** enum_moniker) {
  DCHECK(enum_moniker);

  ComPtr<ICreateDevEnum> dev_enum;
  HRESULT hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                                  IID_PPV_ARGS(&dev_enum));
  if (FAILED(hr)) {
    DLOG(ERROR) << ::logging::SystemErrorCodeToString(hr);
    return hr;
  }

  hr = dev_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                       enum_moniker, 0);
  return hr;
}

// Finds and creates a DirectShow Video Capture filter matching the |device_id|.
// static
HRESULT DshowCamera::GetDeviceFilter(const std::string& device_id,
                                     IBaseFilter** filter) {
  DCHECK(filter);

  ComPtr<IEnumMoniker> enum_moniker;
  HRESULT hr = EnumerateDirectShowDevices(enum_moniker.GetAddressOf());
  // CreateClassEnumerator returns S_FALSE on some Windows OS
  // when no camera exist. Therefore the FAILED macro can't be used.
  if (hr != S_OK) return hr;

  ComPtr<IBaseFilter> capture_filter;
  for (ComPtr<IMoniker> moniker;
       enum_moniker->Next(1, moniker.GetAddressOf(), NULL) == S_OK;
       moniker.Reset()) {
    ComPtr<IPropertyBag> prop_bag;
    hr = moniker->BindToStorage(0, 0, IID_PPV_ARGS(&prop_bag));
    if (FAILED(hr)) continue;

    // Find |device_id| via DevicePath, Description or FriendlyName, whichever
    // is available first and is a VT_BSTR (i.e. String) type.
    static const wchar_t* kPropertyNames[] = {L"DevicePath", L"Description",
                                              L"FriendlyName"};

    ScopedVariant name;
    for (const auto* property_name : kPropertyNames) {
      prop_bag->Read(property_name, name.Receive(), 0);
      if (name.type() == VT_BSTR) break;
    }

    if (name.type() == VT_BSTR) {
      const std::string device_path(base::SysWideToUTF8(V_BSTR(name.ptr())));
      if (device_path.compare(device_id) == 0) {
        // We have found the requested device
        hr = moniker->BindToObject(0, 0, IID_PPV_ARGS(&capture_filter));
        DLOG_IF(ERROR, FAILED(hr)) << "Failed to bind camera filter: "
                                   << logging::SystemErrorCodeToString(hr);
        break;
      }
    }
  }

  *filter = capture_filter.Detach();
  if (!*filter && SUCCEEDED(hr)) hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

  return hr;
}

// Finds an IPin on an IBaseFilter given the direction, Category and/or Major
// Type. If either |category| or |major_type| are GUID_NULL, they are ignored.
// static
ComPtr<IPin> DshowCamera::GetPin(IBaseFilter* filter, PIN_DIRECTION pin_dir,
                                 REFGUID category, REFGUID major_type) {
  ComPtr<IPin> pin;
  ComPtr<IEnumPins> pin_enum;
  HRESULT hr = filter->EnumPins(pin_enum.GetAddressOf());
  if (pin_enum.Get() == NULL) return pin;

  // Get first unconnected pin.
  hr = pin_enum->Reset();  // set to first pin
  while ((hr = pin_enum->Next(1, pin.GetAddressOf(), NULL)) == S_OK) {
    PIN_DIRECTION this_pin_dir = static_cast<PIN_DIRECTION>(-1);
    hr = pin->QueryDirection(&this_pin_dir);
    if (pin_dir == this_pin_dir) {
      if ((category == GUID_NULL || PinMatchesCategory(pin.Get(), category)) &&
          (major_type == GUID_NULL ||
           PinMatchesMajorType(pin.Get(), major_type))) {
        return pin;
      }
    }
    pin.Reset();
  }

  DCHECK(!pin.Get());
  return pin;
}

// static
void DshowCamera::GetDeviceCapabilityList(const std::string& device_id,
                                          bool query_detailed_frame_rates,
                                          CapabilityList* out_capability_list) {
  ComPtr<IBaseFilter> capture_filter;
  HRESULT hr = GetDeviceFilter(device_id, capture_filter.GetAddressOf());
  if (!capture_filter.Get()) {
    DLOG(ERROR) << "Failed to create capture filter: "
                << logging::SystemErrorCodeToString(hr);
    return;
  }

  ComPtr<IPin> output_capture_pin(GetPin(capture_filter.Get(), PINDIR_OUTPUT,
                                         PIN_CATEGORY_CAPTURE, GUID_NULL));
  if (!output_capture_pin.Get()) {
    DLOG(ERROR) << "Failed to get capture output pin";
    return;
  }

  GetPinCapabilityList(capture_filter, output_capture_pin,
                       query_detailed_frame_rates, out_capability_list);
}

// static
void DshowCamera::GetPinCapabilityList(ComPtr<IBaseFilter> capture_filter,
                                       ComPtr<IPin> output_capture_pin,
                                       bool query_detailed_frame_rates,
                                       CapabilityList* out_capability_list) {
  ComPtr<IAMStreamConfig> stream_config;
  HRESULT hr = output_capture_pin.CopyTo(stream_config.GetAddressOf());
  if (FAILED(hr)) {
    DLOG(ERROR) << "Failed to get IAMStreamConfig interface from "
                   "capture device: "
                << logging::SystemErrorCodeToString(hr);
    return;
  }

  // Get interface used for getting the frame rate.
  ComPtr<IAMVideoControl> video_control;
  hr = capture_filter.CopyTo(video_control.GetAddressOf());

  int count = 0, size = 0;
  hr = stream_config->GetNumberOfCapabilities(&count, &size);
  if (FAILED(hr)) {
    DLOG(ERROR) << "GetNumberOfCapabilities failed: "
                << logging::SystemErrorCodeToString(hr);
    return;
  }

  std::unique_ptr<BYTE[]> caps(new BYTE[size]);
  for (int i = 0; i < count; ++i) {
    DshowCamera::ScopedMediaType media_type;
    hr = stream_config->GetStreamCaps(i, media_type.Receive(), caps.get());
    // GetStreamCaps() may return S_FALSE, so don't use FAILED() or SUCCEED()
    // macros here since they'll trigger incorrectly.
    if (hr != S_OK || !media_type.get()) {
      DLOG(ERROR) << "GetStreamCaps failed: "
                  << logging::SystemErrorCodeToString(hr);
      return;
    }

    if (media_type->majortype == MEDIATYPE_Video &&
        media_type->formattype == FORMAT_VideoInfo) {
      CameraFormat camera_format;
      camera_format.set_pixel_format(
          CameraFormat::FromDshowMediaSubtype(media_type->subtype));
      if (camera_format.pixel_format() == PIXEL_FORMAT_UNKNOWN) continue;
      VIDEOINFOHEADER* h =
          reinterpret_cast<VIDEOINFOHEADER*>(media_type->pbFormat);
      camera_format.SetSize(h->bmiHeader.biWidth, h->bmiHeader.biHeight);

      std::vector<float> frame_rates;
      if (query_detailed_frame_rates && video_control.Get()) {
        // Try to get a better |time_per_frame| from IAMVideoControl. If not,
        // use the value from VIDEOINFOHEADER.
        ScopedCoMem<LONGLONG> time_per_frame_list;
        LONG list_size = 0;
        const SIZE size = {camera_format.width(), camera_format.height()};
        hr = video_control->GetFrameRateList(output_capture_pin.Get(), i, size,
                                             &list_size, &time_per_frame_list);
        // Sometimes |list_size| will be > 0, but time_per_frame_list will be
        // NULL. Some drivers may return an HRESULT of S_FALSE which
        // SUCCEEDED() translates into success, so explicitly check S_OK. See
        // http://crbug.com/306237.
        if (hr == S_OK && list_size > 0 && time_per_frame_list) {
          for (int k = 0; k < list_size; k++) {
            LONGLONG time_per_frame = *(time_per_frame_list.get() + k);
            if (time_per_frame <= 0) continue;
            frame_rates.push_back(kSecondsToReferenceTime /
                                  static_cast<float>(time_per_frame));
          }
        }
      }

      if (frame_rates.empty() && h->AvgTimePerFrame > 0) {
        frame_rates.push_back(kSecondsToReferenceTime /
                              static_cast<float>(h->AvgTimePerFrame));
      }
      if (frame_rates.empty()) frame_rates.push_back(0.0f);

      for (const auto& frame_rate : frame_rates) {
        camera_format.set_frame_rate(frame_rate);
        out_capability_list->emplace_back(i, camera_format, h->bmiHeader);
      }
    }
  }
}

Status DshowCamera::InitializeVideoAndCameraControls() {
  ComPtr<IKsTopologyInfo> info;
  HRESULT hr = capture_filter_.CopyTo(info.GetAddressOf());
  if (FAILED(hr))
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to obtain the topology info", hr));

  DWORD num_nodes = 0;
  hr = info->get_NumNodes(&num_nodes);
  if (FAILED(hr))
    return errors::Unavailable(
        MESSAGE_WITH_HRESULT("Failed to obtain the number of nodes.", hr));

  // Every UVC camera is expected to have a single ICameraControl and a single
  // IVideoProcAmp nodes, and both are needed; ignore any unlikely later ones.
  GUID node_type;
  for (size_t i = 0; i < num_nodes; i++) {
    info->get_NodeType(i, &node_type);
    if (IsEqualGUID(node_type, KSNODETYPE_VIDEO_CAMERA_TERMINAL)) {
      hr = info->CreateNodeInstance(i, IID_PPV_ARGS(&camera_control_));
      if (SUCCEEDED(hr)) break;
      if (FAILED(hr))
        return errors::Unavailable(
            MESSAGE_WITH_HRESULT("Failed to retrieve the ICameraControl.", hr));
    }
  }
  for (size_t i = 0; i < num_nodes; i++) {
    info->get_NodeType(i, &node_type);
    if (IsEqualGUID(node_type, KSNODETYPE_VIDEO_PROCESSING)) {
      hr = info->CreateNodeInstance(i, IID_PPV_ARGS(&video_control_));
      if (SUCCEEDED(hr)) break;
      if (FAILED(hr))
        return errors::Unavailable(
            MESSAGE_WITH_HRESULT("Failed to retrieve the IVideoProcAmp.", hr));
    }
  }

  if (video_control_ && camera_control_) {
    return Status::OK();
  } else {
    return errors::Unavailable(
        "video_control_ or camera_control_ is not initialized.");
  }
}

namespace {

CameraSettingsMode ValueToMode(long flag, bool camera_control) {
  if (camera_control) {
    if (flag & CameraControl_Flags_Auto)
      return CameraSettingsMode::CAMERA_SETTINGS_MODE_AUTO;
    else if (flag & CameraControl_Flags_Manual)
      return CameraSettingsMode::CAMERA_SETTINGS_MODE_MANUAL;
  } else {
    if (flag & VideoProcAmp_Flags_Auto)
      return CameraSettingsMode::CAMERA_SETTINGS_MODE_AUTO;
    else if (flag & VideoProcAmp_Flags_Manual)
      return CameraSettingsMode::CAMERA_SETTINGS_MODE_MANUAL;
  }

  return CameraSettingsMode::CAMERA_SETTINGS_MODE_NONE;
}

}  // namespace

void DshowCamera::GetCameraSetting(long property,
                                   CameraSettingsModeValue* value,
                                   bool camera_control) {
  HRESULT hr;
  long min, max, step, default_, flag, v;
  if (camera_control) {
    tagCameraControlProperty tag =
        static_cast<tagCameraControlProperty>(property);
    hr = GetOptionRangedValue(tag, &min, &max, &step, &default_, &flag);
  } else {
    tagVideoProcAmpProperty tag =
        static_cast<tagVideoProcAmpProperty>(property);
    hr = GetOptionRangedValue(tag, &min, &max, &step, &default_, &flag);
  }
  if (FAILED(hr)) {
    value->Clear();
    return;
  }
  value->add_modes(CameraSettingsMode::CAMERA_SETTINGS_MODE_AUTO);
  value->add_modes(CameraSettingsMode::CAMERA_SETTINGS_MODE_MANUAL);
  value->set_default_(ValueToMode(flag, camera_control));

  if (camera_control) {
    tagCameraControlProperty tag =
        static_cast<tagCameraControlProperty>(property);
    hr = GetOptionValue(tag, &v, &flag);
  } else {
    tagVideoProcAmpProperty tag =
        static_cast<tagVideoProcAmpProperty>(property);
    hr = GetOptionValue(tag, &v, &flag);
  }
  if (FAILED(hr)) {
    value->Clear();
    return;
  }
  value->set_current(ValueToMode(flag, camera_control));
}

void DshowCamera::GetCameraSetting(long property,
                                   CameraSettingsRangedValue* value,
                                   bool camera_control) {
  HRESULT hr;
  long min, max, step, default_, flag, v;
  if (camera_control) {
    tagCameraControlProperty tag =
        static_cast<tagCameraControlProperty>(property);
    hr = GetOptionRangedValue(tag, &min, &max, &step, &default_, &flag);
  } else {
    tagVideoProcAmpProperty tag =
        static_cast<tagVideoProcAmpProperty>(property);
    hr = GetOptionRangedValue(tag, &min, &max, &step, &default_, &flag);
  }
  if (FAILED(hr)) {
    value->Clear();
    return;
  }
  value->set_min(static_cast<int64_t>(min));
  value->set_max(static_cast<int64_t>(max));
  value->set_step(static_cast<int64_t>(step));
  value->set_default_(static_cast<int64_t>(default_));
  value->set_flags(static_cast<int64_t>(flag));

  if (camera_control) {
    tagCameraControlProperty tag =
        static_cast<tagCameraControlProperty>(property);
    hr = GetOptionValue(tag, &v, &flag);
  } else {
    tagVideoProcAmpProperty tag =
        static_cast<tagVideoProcAmpProperty>(property);
    hr = GetOptionValue(tag, &v, &flag);
  }
  if (FAILED(hr)) {
    value->Clear();
    return;
  }
  value->set_current(static_cast<int64_t>(v));
}

HRESULT DshowCamera::GetOptionRangedValue(tagCameraControlProperty tag,
                                          long* min, long* max, long* step,
                                          long* default_, long* flag) {
  switch (tag) {
    case CameraControl_Exposure: {
      return camera_control_->getRange_Exposure(min, max, step, default_, flag);
    }
    default:
      return -1;
  }
}

HRESULT DshowCamera::GetOptionRangedValue(tagVideoProcAmpProperty tag,
                                          long* min, long* max, long* step,
                                          long* default_, long* flag) {
  switch (tag) {
    case VideoProcAmp_WhiteBalance: {
      return video_control_->getRange_WhiteBalance(min, max, step, default_,
                                                   flag);
    }
    case VideoProcAmp_Brightness: {
      return video_control_->getRange_Brightness(min, max, step, default_,
                                                 flag);
    }
    case VideoProcAmp_Contrast: {
      return video_control_->getRange_Contrast(min, max, step, default_, flag);
    }
    case VideoProcAmp_Saturation: {
      return video_control_->getRange_Saturation(min, max, step, default_,
                                                 flag);
    }
    case VideoProcAmp_Sharpness: {
      return video_control_->getRange_Sharpness(min, max, step, default_, flag);
    }
    case VideoProcAmp_Hue: {
      return video_control_->getRange_Hue(min, max, step, default_, flag);
    }
    case VideoProcAmp_Gain: {
      return video_control_->getRange_Gain(min, max, step, default_, flag);
    }
    case VideoProcAmp_Gamma: {
      return video_control_->getRange_Gamma(min, max, step, default_, flag);
    }
    default:
      return -1;
  }
}

HRESULT DshowCamera::GetOptionValue(tagCameraControlProperty tag, long* value,
                                    long* flag) {
  switch (tag) {
    case CameraControl_Exposure: {
      return camera_control_->get_Exposure(value, flag);
    }
    default:
      return -1;
  }
}

HRESULT DshowCamera::GetOptionValue(tagVideoProcAmpProperty tag, long* value,
                                    long* flag) {
  switch (tag) {
    case VideoProcAmp_WhiteBalance: {
      return video_control_->get_WhiteBalance(value, flag);
    }
    case VideoProcAmp_Brightness: {
      return video_control_->get_Brightness(value, flag);
    }
    case VideoProcAmp_Contrast: {
      return video_control_->get_Contrast(value, flag);
    }
    case VideoProcAmp_Saturation: {
      return video_control_->get_Saturation(value, flag);
    }
    case VideoProcAmp_Sharpness: {
      return video_control_->get_Sharpness(value, flag);
    }
    case VideoProcAmp_Hue: {
      return video_control_->get_Hue(value, flag);
    }
    case VideoProcAmp_Gain: {
      return video_control_->get_Gain(value, flag);
    }
    case VideoProcAmp_Gamma: {
      return video_control_->get_Gamma(value, flag);
    }
    default:
      return -1;
  }
}

#undef MESSAGE_WITH_HRESULT

}  // namespace felicia