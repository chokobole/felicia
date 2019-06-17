#include "felicia/drivers/vendors/zed/zed_camera.h"

#include "third_party/chromium/base/bind.h"

#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/lib/synchronization/scoped_event_signaller.h"
#include "felicia/drivers/camera/camera_errors.h"
#include "felicia/drivers/vendors/zed/zed_camera_frame.h"

namespace felicia {

#define MESSAGE_WITH_ERROR_CODE(text, err) \
  ::base::StringPrintf("%s :%s.", text, ::sl::toString(err).c_str())

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
    : StereoCameraInterface(camera_descriptor),
      thread_("ZedCameraThread"),
      is_stopping_(false) {}

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

  const ZedCapability* capability =
      GetBestMatchedCapability(requested_camera_format);
  if (!capability) return errors::NoVideoCapbility();

  ScopedCamera camera;
  ::sl::InitParameters params;
  params.camera_fps = capability->frame_rate;
  params.camera_resolution = capability->resolution;
  camera_format_ = ConvertToCameraFormat(*capability);
  if (requested_camera_format.convert_to_argb()) {
    camera_format_.set_convert_to_argb(true);
  }
  camera_->close();
  Status s = OpenCamera(camera_descriptor_, params, &camera);
  if (!s.ok()) return s;

  camera_ = std::move(camera);
  thread_.Start();
  left_camera_frame_callback_ = left_camera_frame_callback;
  right_camera_frame_callback_ = right_camera_frame_callback;
  status_callback_ = status_callback;
  camera_state_.ToStarted();
  {
    ::base::AutoLock l(lock_);
    is_stopping_ = false;
  }

  if (thread_.task_runner()->BelongsToCurrentThread()) {
    DoGrab();
  } else {
    thread_.task_runner()->PostTask(
        FROM_HERE, ::base::BindOnce(&ZedCamera::DoGrab, AsWeakPtr()));
  }

  return Status::OK();
}

Status ZedCamera::Stop() {
  if (!camera_state_.IsStarted()) {
    return camera_state_.InvalidStateError();
  }

  {
    ::base::AutoLock l(lock_);
    is_stopping_ = true;
  }
  Status s;
  if (thread_.task_runner()->BelongsToCurrentThread()) {
    DoStop(nullptr, &s);
  } else {
    ::base::WaitableEvent* event = new ::base::WaitableEvent();
    thread_.task_runner()->PostTask(
        FROM_HERE,
        ::base::BindOnce(&ZedCamera::DoStop, AsWeakPtr(), event, &s));
    event->Wait();
    delete event;
  }

  if (thread_.IsRunning()) thread_.Stop();

  left_camera_frame_callback_.Reset();
  right_camera_frame_callback_.Reset();
  status_callback_.Reset();

  camera_state_.ToStopped();
  return s;
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

void ZedCamera::DoGrab() {
  DCHECK(thread_.task_runner()->BelongsToCurrentThread());

  ::sl::RuntimeParameters params;
  {
    ::base::AutoLock l(lock_);
    if (is_stopping_) return;
  }

  ::sl::ERROR_CODE err = camera_->grab(params);
  if (err != ::sl::SUCCESS && err != ::sl::ERROR_CODE_NOT_A_NEW_FRAME) {
    status_callback_.Run(
        errors::Unavailable(MESSAGE_WITH_ERROR_CODE("Failed to grab", err)));
    return;
  }

  ::sl::Mat left_image;
  camera_->retrieveImage(left_image, ::sl::VIEW_LEFT);
  ::sl::Mat right_image;
  camera_->retrieveImage(right_image, ::sl::VIEW_RIGHT);

  left_camera_frame_callback_.Run(
      ConverToCameraFrame(left_image, camera_format_));
  right_camera_frame_callback_.Run(
      ConverToCameraFrame(right_image, camera_format_));

  thread_.task_runner()->PostTask(
      FROM_HERE, ::base::BindOnce(&ZedCamera::DoGrab, AsWeakPtr()));
}

void ZedCamera::DoStop(::base::WaitableEvent* event, Status* status) {
  DCHECK(thread_.task_runner()->BelongsToCurrentThread());

  camera_->close();

  *status = Status::OK();
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
    return errors::Unavailable(
        MESSAGE_WITH_ERROR_CODE("Failed to open camera", err));
  }
  return Status::OK();
}

#undef MESSAGE_WITH_ERROR_CODE

}  // namespace felicia