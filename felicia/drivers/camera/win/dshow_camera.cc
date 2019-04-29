// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Modified by Wonyong Kim(chokobole33@gmail.com)

#include "felicia/drivers/camera/win/dshow_camera.h"

#include "third_party/chromium/base/strings/sys_string_conversions.h"
#include "third_party/chromium/base/win/scoped_co_mem.h"
#include "third_party/chromium/base/win/scoped_variant.h"

#include "felicia/drivers/camera/camera_errors.h"
#include "felicia/drivers/camera/timestamp_constants.h"
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

DshowCamera::DshowCamera(const CameraDescriptor& descriptor)
    : descriptor_(descriptor) {}

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
  return errors::Unimplemented("Not implemented yet.");
}

Status DshowCamera::Init() {
  HRESULT hr =
      GetDeviceFilter(descriptor_.device_id(), capture_filter_.GetAddressOf());
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
  if (FAILED(hr)) return errors::FailedtoAddSinkFilter(hr);

  StatusOr<CameraFormat> status_or = GetCurrentCameraFormat();
  if (!status_or.ok()) return status_or.status();

  camera_format_ = status_or.ValueOrDie();
  DLOG(INFO) << "Default Format: " << camera_format_.ToString();

  return Status::OK();
}

Status DshowCamera::Start(CameraFrameCallback camera_frame_callback,
                          StatusCallback status_callback) {
  camera_frame_callback_ = camera_frame_callback;
  status_callback_ = status_callback;

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

  return Status::OK();
}

Status DshowCamera::Close() {
  return errors::Unimplemented("Not implemented yet.");
}

Status DshowCamera::GetSupportedCameraFormats(CameraFormats* camera_formats) {
  return errors::Unimplemented("Not implemented yet.");
}

StatusOr<CameraFormat> DshowCamera::GetCurrentCameraFormat() {
  ComPtr<IAMStreamConfig> stream_config;
  HRESULT hr = output_capture_pin_.CopyTo(stream_config.GetAddressOf());
  if (FAILED(hr)) {
    return errors::FailedToGetIAMStreamConfig(hr);
  }

  DshowCamera::ScopedMediaType media_type;
  hr = stream_config->GetFormat(media_type.Receive());
  if (FAILED(hr)) {
    return errors::FailedToGetFormatFromIAMStreamConfig(hr);
  }

  if (!(media_type->majortype == MEDIATYPE_Video &&
        media_type->formattype == FORMAT_VideoInfo)) {
    return errors::IsNotAVideoType();
  }

  CameraFormat camera_format;
  camera_format.set_pixel_format(
      CameraFormat::FromMediaSubtype(media_type->subtype));
  VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(media_type->pbFormat);
  camera_format.SetSize(h->bmiHeader.biWidth, h->bmiHeader.biHeight);

  // Get interface used for getting the frame rate.
  // ComPtr<IAMVideoControl> video_control;
  // hr = capture_filter_.CopyTo(video_control.GetAddressOf());
  // if (FAILED(hr)) {
  //   return errors::FailedToGetIAMVideoControl(hr);
  // }

  return camera_format;
}

Status DshowCamera::SetCameraFormat(const CameraFormat& format) {
  return errors::Unimplemented("Not implemented yet.");
}

void DshowCamera::FrameReceived(const uint8_t* buffer, int length,
                                const CameraFormat& format,
                                ::base::TimeDelta timestamp) {
  auto new_buffer = std::unique_ptr<uint8_t>(new uint8_t[length]);
  memcpy(new_buffer.get(), buffer, length);
  CameraFrame camera_frame(std::move(new_buffer), format);

  if (first_ref_time_.is_null()) first_ref_time_ = base::TimeTicks::Now();

  // There is a chance that the platform does not provide us with the timestamp,
  // in which case, we use reference time to calculate a timestamp.
  if (timestamp == kNoTimestamp)
    timestamp = base::TimeTicks::Now() - first_ref_time_;

  camera_frame.set_timestamp(timestamp);
  camera_frame_callback_.Run(std::move(camera_frame));
}

void DshowCamera::FrameDropped(const Status& s) { status_callback_.Run(s); }

// Finds and creates a DirectShow Video Capture filter matching the |device_id|.
// static
HRESULT DshowCamera::GetDeviceFilter(const std::string& device_id,
                                     IBaseFilter** filter) {
  DCHECK(filter);

  ComPtr<ICreateDevEnum> dev_enum;
  HRESULT hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                                  IID_PPV_ARGS(&dev_enum));
  if (FAILED(hr)) return hr;

  ComPtr<IEnumMoniker> enum_moniker;
  hr = dev_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                       enum_moniker.GetAddressOf(), 0);
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

}  // namespace felicia