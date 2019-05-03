// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)
// Followings are taken and modified from
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/video_capture_device_win.cc
// https://github.com/chromium/chromium/blob/5db095c2653f332334d56ad739ae5fe1053308b1/media/capture/video/win/video_capture_device_factory_win.cc

#include "felicia/drivers/camera/win/dshow_camera.h"

#include "third_party/chromium/base/strings/sys_string_conversions.h"
#include "third_party/chromium/base/win/scoped_co_mem.h"
#include "third_party/chromium/base/win/scoped_variant.h"

#include "felicia/drivers/camera/camera_buffer.h"
#include "felicia/drivers/camera/camera_errors.h"
#include "felicia/drivers/camera/timestamp_constants.h"
#include "felicia/drivers/camera/win/camera_util.h"
#include "felicia/drivers/camera/win/sink_input_pin.h"

using base::win::ScopedCoMem;
using base::win::ScopedVariant;
using Microsoft::WRL::ComPtr;

namespace felicia {

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

    const std::string device_name(::base::SysWideToUTF8(V_BSTR(name.ptr())));

    name.Reset();
    hr = prop_bag->Read(L"DevicePath", name.Receive(), 0);
    std::string id;
    if (FAILED(hr) || name.type() != VT_BSTR) {
      id = device_name;
    } else {
      DCHECK_EQ(name.type(), VT_BSTR);
      id = ::base::SysWideToUTF8(V_BSTR(name.ptr()));
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
  if (FAILED(hr)) return errors::FailedToCreateCaptureFilter(hr);

  output_capture_pin_ = GetPin(capture_filter_.Get(), PINDIR_OUTPUT,
                               PIN_CATEGORY_CAPTURE, GUID_NULL);
  if (!output_capture_pin_.Get()) return errors::FaieldToGetCaptureOutputPin();

  // Create the sink filter used for receiving Captured frames.
  sink_filter_ = new SinkFilter(this);
  if (sink_filter_.get() == NULL) {
    return errors::FailedToCreateSinkFilter();
  }

  input_sink_pin_ = sink_filter_->GetPin(0);

  hr = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&graph_builder_));
  if (FAILED(hr)) return errors::FailedToCreateCaptureFilter(hr);

  hr = ::CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
                          IID_PPV_ARGS(&capture_graph_builder_));
  if (FAILED(hr)) return errors::FailedToCreateCaptureGraphBuilder(hr);

  hr = capture_graph_builder_->SetFiltergraph(graph_builder_.Get());
  if (FAILED(hr)) return errors::FailedToSetGraphBuilderFilterGraph(hr);

  hr = graph_builder_.CopyTo(media_control_.GetAddressOf());
  if (FAILED(hr)) return errors::FailedToCreateMediaControlBuilder(hr);

  hr = graph_builder_->AddFilter(capture_filter_.Get(), NULL);
  if (FAILED(hr)) return errors::FailedToAddCaptureFilter(hr);

  hr = graph_builder_->AddFilter(sink_filter_.get(), NULL);
  if (FAILED(hr)) return errors::FailedToAddSinkFilter(hr);

  was_set_camera_format_ = false;
  // This should be before calling GetCurrentCameraFormat()
  camera_state_.ToInitialized();

  StatusOr<CameraFormat> status_or = GetCurrentCameraFormat();
  if (!status_or.ok()) return status_or.status();
  camera_format_ = status_or.ValueOrDie();
  DLOG(INFO) << "Default Format: " << camera_format_.ToString();

  return CreateCapabilityMap();
}

Status DshowCamera::Start(CameraFrameCallback camera_frame_callback,
                          StatusCallback status_callback) {
  if (!camera_state_.IsInitialized()) {
    return camera_state_.InvalidStateError();
  }

  // if (media_type->subtype == kMediaSubTypeHDYC) {
  //   // HDYC pixel format, used by the DeckLink capture card, needs an AVI
  //   // decompressor filter after source, let |graph_builder_| add it.
  //   hr = graph_builder_->Connect(output_capture_pin_.Get(),
  //                                input_sink_pin_.Get());
  // } else {
  HRESULT hr = graph_builder_->ConnectDirect(output_capture_pin_.Get(),
                                             input_sink_pin_.Get(), NULL);
  // }

  if (FAILED(hr)) return errors::FailedToConnectTheCaptureGraph(hr);

  hr = media_control_->Pause();
  if (FAILED(hr)) return errors::FailedToPause(hr);

  // Start capturing.
  hr = media_control_->Run();
  if (FAILED(hr)) return errors::FailedToRun(hr);

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
    return errors::FailedToStop(hr);
  }

  graph_builder_->Disconnect(output_capture_pin_.Get());
  graph_builder_->Disconnect(input_sink_pin_.Get());

  camera_frame_callback_.Reset();
  status_callback_.Reset();
  camera_state_.ToStopped();

  return Status::OK();
}

StatusOr<CameraFormat> DshowCamera::GetCurrentCameraFormat() {
  if (camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }

  if (was_set_camera_format_) {
    return camera_format_;
  }

  ComPtr<IAMStreamConfig> stream_config;
  HRESULT hr = output_capture_pin_.CopyTo(stream_config.GetAddressOf());
  if (FAILED(hr)) {
    return errors::FailedToGetIAMStreamConfig(hr);
  }

  DshowCamera::ScopedMediaType media_type;
  hr = stream_config->GetFormat(media_type.Receive());
  if (FAILED(hr)) {
    return errors::FailedToGetFormat(hr);
  }

  if (!(media_type->majortype == MEDIATYPE_Video &&
        media_type->formattype == FORMAT_VideoInfo)) {
    return errors::IsNotAVideoType();
  }

  CameraFormat camera_format;
  camera_format.set_pixel_format(
      CameraFormat::FromDshowMediaSubtype(media_type->subtype));
  VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(media_type->pbFormat);
  camera_format.SetSize(h->bmiHeader.biWidth, h->bmiHeader.biHeight);
  camera_format.set_frame_rate(kSecondsToReferenceTime /
                               static_cast<float>(h->AvgTimePerFrame));

  return camera_format;
}

Status DshowCamera::SetCameraFormat(const CameraFormat& camera_format) {
  if (!camera_state_.IsInitialized()) {
    return camera_state_.InvalidStateError();
  }

  // Get the camera capability that best match the requested format.
  const Capability* found_capability =
      GetBestMatchedCapability(camera_format, capabilities_);

  if (!found_capability) {
    return errors::FailedToGetBestMatchedCapability();
  }

  // Reduce the frame rate if the requested frame rate is lower
  // than the capability.
  const float frame_rate =
      std::min(camera_format.frame_rate(),
               found_capability->supported_format.frame_rate());

  ComPtr<IAMStreamConfig> stream_config;
  HRESULT hr = output_capture_pin_.CopyTo(stream_config.GetAddressOf());
  if (FAILED(hr)) {
    return errors::FailedToGetIAMStreamConfig(hr);
  }

  int count = 0, size = 0;
  hr = stream_config->GetNumberOfCapabilities(&count, &size);
  if (FAILED(hr)) {
    return errors::FailedToGetNumberOfCapabilities(hr);
  }

  std::unique_ptr<BYTE[]> caps(new BYTE[size]);
  ScopedMediaType media_type;

  // Get the windows capability from the capture device.
  // GetStreamCaps can return S_FALSE which we consider an error. Therefore the
  // FAILED macro can't be used.
  hr = stream_config->GetStreamCaps(found_capability->media_type_index,
                                    media_type.Receive(), caps.get());
  if (hr != S_OK) {
    return errors::FailedToGetStreamCaps(hr);
  }

  // Set the sink filter to request this format.
  sink_filter_->SetRequestedMediaFormat(camera_format.pixel_format(),
                                        camera_format.frame_rate(),
                                        found_capability->info_header);
  // Order the capture device to use this format.
  hr = stream_config->SetFormat(media_type.get());
  if (hr != S_OK) {
    return errors::FailedToSetFormat(hr);
  }

  camera_format_ = found_capability->supported_format;
  was_set_camera_format_ = true;

  return Status::OK();
}

void DshowCamera::FrameReceived(const uint8_t* buffer, int length,
                                const CameraFormat& camera_format,
                                ::base::TimeDelta timestamp) {
  CameraBuffer camera_buffer(const_cast<uint8_t*>(buffer), length);
  camera_buffer.set_payload(length);
  ::base::Optional<CameraFrame> argb_frame =
      ConvertToARGB(camera_buffer, camera_format);
  if (argb_frame.has_value()) {
    if (first_ref_time_.is_null()) first_ref_time_ = base::TimeTicks::Now();

    // There is a chance that the platform does not provide us with the
    // timestamp, in which case, we use reference time to calculate a timestamp.
    if (timestamp == kNoTimestamp)
      timestamp = base::TimeTicks::Now() - first_ref_time_;

    argb_frame.value().set_timestamp(timestamp);
    camera_frame_callback_.Run(std::move(argb_frame.value()));
  } else {
    status_callback_.Run(errors::FailedToConvertToARGB());
  }
}

void DshowCamera::FrameDropped(const Status& s) { status_callback_.Run(s); }

Status DshowCamera::CreateCapabilityMap() {
  GetPinCapabilityList(capture_filter_, output_capture_pin_,
                       true /* query_detailed_frame_rates */, &capabilities_);
  if (capabilities_.empty()) return errors::NoVideoCapbility();
  return Status::OK();
}

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
      if (camera_format.pixel_format() == CameraFormat::PIXEL_FORMAT_UNKNOWN)
        continue;
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

}  // namespace felicia