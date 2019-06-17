#include "felicia/drivers/vendors/zed/zed_camera.h"

#include "felicia/core/lib/error/errors.h"
#include "felicia/core/lib/strings/str_util.h"

namespace felicia {

ZedCamera::ScopedCamera::ScopedCamera()
    : camera_(std::make_unique<::sl::Camera>()) {}

ZedCamera::ScopedCamera::ScopedCamera(ZedCamera::ScopedCamera&& other)
    : camera_(std::move(other.camera_)) {}

void ZedCamera::ScopedCamera::operator=(ZedCamera::ScopedCamera&& other) {
  if (camera_ && camera_->isOpened()) {
    camera_->close();
  }
  camera_ = std::move(other.camera_);
}

ZedCamera::ScopedCamera::~ScopedCamera() {
  if (camera_ && camera_->isOpened()) {
    camera_->close();
  }
}

ZedCamera::ZedCamera(const CameraDescriptor& camera_descriptor)
    : StereoCameraInterface(camera_descriptor) {}

ZedCamera::~ZedCamera() = default;

Status ZedCamera::Init() {
  ScopedCamera camera;
  ::sl::InitParameters params;
  Status s = OpenCamera(camera_descriptor_, params, &camera);
  if (!s.ok()) return s;

  camera_ = std::move(camera);
  camera_state_.ToInitialized();
  return Status::OK();
}

Status ZedCamera::Start(const CameraFormat& requested_camera_format,
                        CameraFrameCallback left_camera_frame_callback,
                        CameraFrameCallback right_camera_frame_callback,
                        StatusCallback status_callback) {
  if (!camera_state_.IsInitialized()) {
    return camera_state_.InvalidStateError();
  }

  camera_state_.ToStarted();
  return errors::Unimplemented("Not implemented yet.");
}

Status ZedCamera::Stop() {
  if (!camera_state_.IsStarted()) {
    return camera_state_.InvalidStateError();
  }
  camera_state_.ToStopped();
  return errors::Unimplemented("Not implemented yet.");
}

Status ZedCamera::SetCameraSettings(const CameraSettings& camera_settings) {
  if (camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }

  if (camera_settings.has_white_balance_mode()) {
    int value =
        camera_settings.white_balance_mode() == CAMERA_SETTINGS_MODE_AUTO ? 1
                                                                          : 0;
    camera_->setCameraSettings(::sl::CAMERA_SETTINGS_AUTO_WHITEBALANCE, value);
  }

  if (camera_settings.has_color_temperature()) {
    bool can_set = false;
    {
      int value =
          camera_->getCameraSettings(::sl::CAMERA_SETTINGS_AUTO_WHITEBALANCE);
      can_set = value == 1;
    }
    if (can_set) {
      int value = camera_settings.color_temperature();
      camera_->setCameraSettings(::sl::CAMERA_SETTINGS_WHITEBALANCE, value);
    }
  }

  if (camera_settings.has_exposure_time()) {
    int value = camera_settings.exposure_time();
    camera_->setCameraSettings(::sl::CAMERA_SETTINGS_EXPOSURE, value);
  }

  if (camera_settings.has_brightness()) {
    int value = camera_settings.brightness();
    camera_->setCameraSettings(::sl::CAMERA_SETTINGS_BRIGHTNESS, value);
  }

  if (camera_settings.has_contrast()) {
    int value = camera_settings.contrast();
    camera_->setCameraSettings(::sl::CAMERA_SETTINGS_CONTRAST, value);
  }

  if (camera_settings.has_saturation()) {
    int value = camera_settings.saturation();
    camera_->setCameraSettings(::sl::CAMERA_SETTINGS_SATURATION, value);
  }

  if (camera_settings.has_hue()) {
    int value = camera_settings.hue();
    camera_->setCameraSettings(::sl::CAMERA_SETTINGS_HUE, value);
  }

  if (camera_settings.has_gain()) {
    int value = camera_settings.gain();
    camera_->setCameraSettings(::sl::CAMERA_SETTINGS_GAIN, value);
  }

  return Status::OK();
}

Status ZedCamera::GetCameraSettingsInfo(
    CameraSettingsInfoMessage* camera_settings) {
  if (camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }
  GetCameraSetting(::sl::CAMERA_SETTINGS_AUTO_WHITEBALANCE,
                   camera_settings->mutable_white_balance_mode());
  GetCameraSetting(::sl::CAMERA_SETTINGS_WHITEBALANCE,
                   camera_settings->mutable_color_temperature());
  GetCameraSetting(::sl::CAMERA_SETTINGS_EXPOSURE,
                   camera_settings->mutable_exposure_time());
  GetCameraSetting(::sl::CAMERA_SETTINGS_BRIGHTNESS,
                   camera_settings->mutable_brightness());
  GetCameraSetting(::sl::CAMERA_SETTINGS_CONTRAST,
                   camera_settings->mutable_contrast());
  GetCameraSetting(::sl::CAMERA_SETTINGS_SATURATION,
                   camera_settings->mutable_saturation());
  GetCameraSetting(::sl::CAMERA_SETTINGS_HUE, camera_settings->mutable_hue());
  GetCameraSetting(::sl::CAMERA_SETTINGS_GAIN, camera_settings->mutable_gain());
  return Status::OK();
}

namespace {

CameraSettingsMode ValueToMode(int value) {
  return value == 1 ? CameraSettingsMode::CAMERA_SETTINGS_MODE_AUTO
                    : CameraSettingsMode::CAMERA_SETTINGS_MODE_MANUAL;
}

}  // namespace

void ZedCamera::GetCameraSetting(::sl::CAMERA_SETTINGS camera_setting,
                                 CameraSettingsModeValue* value) {
  int ret = camera_->getCameraSettings(camera_setting);
  if (ret == -1) {
    value->Clear();
    return;
  }
  value->add_modes(CameraSettingsMode::CAMERA_SETTINGS_MODE_AUTO);
  value->add_modes(CameraSettingsMode::CAMERA_SETTINGS_MODE_MANUAL);
  value->set_current(ValueToMode(ret));
}

void ZedCamera::GetCameraSetting(::sl::CAMERA_SETTINGS camera_setting,
                                 CameraSettingsRangedValue* value) {
  int ret = camera_->getCameraSettings(camera_setting);
  if (ret == -1) {
    value->Clear();
    return;
  }
  value->set_current(ret);
}

// static
Status ZedCamera::OpenCamera(const CameraDescriptor& camera_descriptor,
                             ::sl::InitParameters& params,
                             ZedCamera::ScopedCamera* camera) {
  int id = -1;
  std::vector<::sl::DeviceProperties> deviceList =
      ::sl::Camera::getDeviceList();
  for (auto& device : deviceList) {
    if (device.camera_state == ::sl::CAMERA_STATE_AVAILABLE &&
        strings::Equals(camera_descriptor.device_id(), device.path.c_str())) {
      id = device.id;
      break;
    }
  }

  if (id == -1) {
    return errors::InvalidArgument(
        "No avaiable camera from given camera_descriptor.");
  } else {
    params.input.setFromCameraID(id);
  }

  ::sl::ERROR_CODE err = camera->get()->open(params);
  if (err != ::sl::SUCCESS) {
    return errors::Unavailable(::base::StringPrintf(
        "Failed to open camera: %s.", ::sl::toString(err).c_str()));
  }
  return Status::OK();
}

}  // namespace felicia