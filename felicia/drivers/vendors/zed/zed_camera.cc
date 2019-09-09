#include "felicia/drivers/vendors/zed/zed_camera.h"

#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/bit_cast.h"

#include "felicia/core/lib/strings/str_util.h"
#include "felicia/core/lib/synchronization/scoped_event_signaller.h"
#include "felicia/core/lib/unit/length.h"
#include "felicia/drivers/camera/camera_errors.h"

namespace felicia {
namespace drivers {

#define MESSAGE_WITH_ERROR_CODE(text, err) \
  base::StringPrintf("%s :%s.", text, sl::toString(err).c_str())

namespace {

uint16_t InMillimeter(sl::UNIT unit, float value) {
  Length length;
  switch (unit) {
    case sl::UNIT_MILLIMETER:
      length = Length::FromMillimeter(static_cast<int64_t>(value));
      break;
    case sl::UNIT_CENTIMETER:
      length = Length::FromCentimeterD(value);
      break;
    case sl::UNIT_METER:
      length = Length::FromMeterD(value);
      break;
    case sl::UNIT_INCH:
      length = Length::FromInchD(value);
      break;
    case sl::UNIT_FOOT:
      length = Length::FromFeetD(value);
      break;
    default:
      NOTREACHED() << "Invalid unit" << sl::toString(unit);
      break;
  }
  return static_cast<uint16_t>(length.InMillimeter());
}

float InMeter(sl::UNIT unit, float value) {
  Length length;
  switch (unit) {
    case sl::UNIT_MILLIMETER:
      length = Length::FromMillimeter(static_cast<int64_t>(value));
      break;
    case sl::UNIT_CENTIMETER:
      length = Length::FromCentimeterD(value);
      break;
    case sl::UNIT_METER:
      length = Length::FromMeterD(value);
      break;
    case sl::UNIT_INCH:
      length = Length::FromInchD(value);
      break;
    case sl::UNIT_FOOT:
      length = Length::FromFeetD(value);
      break;
    default:
      NOTREACHED() << "Invalid unit" << sl::toString(unit);
      break;
  }
  return static_cast<float>(length.InMeter());
}

Coordinate ToCoordinate(sl::COORDINATE_SYSTEM coordinate_system) {
  switch (coordinate_system) {
    case sl::COORDINATE_SYSTEM_IMAGE:
      return Coordinate();
    case sl::COORDINATE_SYSTEM_LEFT_HANDED_Y_UP:
      return Coordinate(Coordinate::COORDINATE_SYSTEM_LEFT_HANDED_Y_UP);
    case sl::COORDINATE_SYSTEM_LEFT_HANDED_Z_UP:
      return Coordinate(Coordinate::COORDINATE_SYSTEM_LEFT_HANDED_Z_UP);
    case sl::COORDINATE_SYSTEM_RIGHT_HANDED_Y_UP:
      return Coordinate(Coordinate::COORDINATE_SYSTEM_RIGHT_HANDED_Y_UP);
    case sl::COORDINATE_SYSTEM_RIGHT_HANDED_Z_UP:
      return Coordinate(Coordinate::COORDINATE_SYSTEM_RIGHT_HANDED_Z_UP);
    case sl::COORDINATE_SYSTEM_RIGHT_HANDED_Z_UP_X_FWD:
      return Coordinate(Coordinate::COORDINATE_SYSTEM_RIGHT_HANDED_Z_UP_X_FWD);
    default:
      NOTREACHED() << "Invalid coordinate system"
                   << sl::toString(coordinate_system);
      return Coordinate();
  }
}

}  // namespace

ZedCamera::ScopedCamera::ScopedCamera()
    : camera_(std::make_unique<sl::Camera>()) {}

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

// static
bool ZedCamera::IsSameId(const std::string& device_id,
                         const std::string& device_id2) {
#if defined(OS_WIN)
  // (device_id looks like \\?usb#vid_...&pid_...&mi_...){...}...
  // Pick inside parenthesis
  return device_id.substr(0, device_id.find_first_of('{')) ==
         device_id2.substr(0, device_id2.find_first_of('{'));
#else
  return device_id == device_id2;
#endif
}

ZedCamera::ZedCamera(const ZedCameraDescriptor& camera_descriptor)
    : StereoCameraInterface(camera_descriptor),
      device_id_(camera_descriptor.id()),
      thread_("ZedCameraThread"),
      is_stopping_(false) {}

ZedCamera::~ZedCamera() = default;

Status ZedCamera::Init() {
  ScopedCamera camera;
  sl::InitParameters params;
  Status s = OpenCamera(device_id_, params, &camera);
  if (!s.ok()) return s;

  camera_ = std::move(camera);
  camera_state_.ToInitialized();
  return Status::OK();
}

Status ZedCamera::Start(const CameraFormat& requested_camera_format,
                        CameraFrameCallback left_camera_frame_callback,
                        CameraFrameCallback right_camera_frame_callback,
                        DepthCameraFrameCallback depth_camera_frame_callback,
                        StatusCallback status_callback) {
  StartParams params;
  params.requested_camera_format = requested_camera_format;
  params.left_camera_frame_callback = left_camera_frame_callback;
  params.right_camera_frame_callback = right_camera_frame_callback;
  params.depth_camera_frame_callback = depth_camera_frame_callback;
  params.status_callback = status_callback;
  return Start(params);
}

Status ZedCamera::Start(const ZedCamera::StartParams& params) {
  if (!camera_state_.IsInitialized()) {
    return camera_state_.InvalidStateError();
  }

  const ZedCapability* capability =
      GetBestMatchedCapability(params.requested_camera_format);
  if (!capability) return errors::NoVideoCapbility();

  init_params_ = params.init_params;
  init_params_.camera_fps = capability->frame_rate;
  init_params_.camera_resolution = capability->resolution;
  coordinate_ = ToCoordinate(init_params_.coordinate_system);
  camera_format_ = ConvertToCameraFormat(*capability);
  depth_camera_format_.set_pixel_format(PIXEL_FORMAT_Z16);
  depth_camera_format_.SetSize(camera_format_.width(), camera_format_.height());
  depth_camera_format_.set_frame_rate(camera_format_.frame_rate());
  camera_->close();
  ScopedCamera camera;
  Status s = OpenCamera(device_id_, init_params_, &camera);
  if (!s.ok()) return s;

  camera_ = std::move(camera);
  thread_.Start();
  left_camera_frame_callback_ = params.left_camera_frame_callback;
  right_camera_frame_callback_ = params.right_camera_frame_callback;
  depth_camera_frame_callback_ = params.depth_camera_frame_callback;
  pointcloud_callback_ = params.pointcloud_callback;
  status_callback_ = params.status_callback;
  runtime_params_ = params.runtime_params;
  camera_state_.ToStarted();
  {
    base::AutoLock l(lock_);
    is_stopping_ = false;
  }
  if (!depth_camera_frame_callback_.is_null()) {
    runtime_params_.enable_depth = true;
  }
  if (!pointcloud_callback_.is_null()) {
    runtime_params_.enable_point_cloud = true;
  }

  if (thread_.task_runner()->BelongsToCurrentThread()) {
    DoGrab();
  } else {
    thread_.task_runner()->PostTask(
        FROM_HERE, base::BindOnce(&ZedCamera::DoGrab, AsWeakPtr()));
  }

  return Status::OK();
}

Status ZedCamera::Stop() {
  if (!camera_state_.IsStarted()) {
    return camera_state_.InvalidStateError();
  }

  {
    base::AutoLock l(lock_);
    is_stopping_ = true;
  }
  Status s;
  if (thread_.task_runner()->BelongsToCurrentThread()) {
    DoStop(nullptr, &s);
  } else {
    base::WaitableEvent* event = new base::WaitableEvent();
    thread_.task_runner()->PostTask(
        FROM_HERE, base::BindOnce(&ZedCamera::DoStop, AsWeakPtr(), event, &s));
    event->Wait();
    delete event;
  }

  if (thread_.IsRunning()) thread_.Stop();

  left_camera_frame_callback_.Reset();
  right_camera_frame_callback_.Reset();
  depth_camera_frame_callback_.Reset();
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
    camera_->setCameraSettings(sl::CAMERA_SETTINGS_AUTO_WHITEBALANCE, value);
  }

  if (camera_settings.has_color_temperature()) {
    bool can_set = false;
    {
      int value =
          camera_->getCameraSettings(sl::CAMERA_SETTINGS_AUTO_WHITEBALANCE);
      can_set = value == 1;
    }
    if (can_set) {
      int value = camera_settings.color_temperature();
      camera_->setCameraSettings(sl::CAMERA_SETTINGS_WHITEBALANCE, value);
    }
  }

  if (camera_settings.has_exposure_time()) {
    int value = camera_settings.exposure_time();
    camera_->setCameraSettings(sl::CAMERA_SETTINGS_EXPOSURE, value);
  }

  if (camera_settings.has_brightness()) {
    int value = camera_settings.brightness();
    camera_->setCameraSettings(sl::CAMERA_SETTINGS_BRIGHTNESS, value);
  }

  if (camera_settings.has_contrast()) {
    int value = camera_settings.contrast();
    camera_->setCameraSettings(sl::CAMERA_SETTINGS_CONTRAST, value);
  }

  if (camera_settings.has_saturation()) {
    int value = camera_settings.saturation();
    camera_->setCameraSettings(sl::CAMERA_SETTINGS_SATURATION, value);
  }

  if (camera_settings.has_hue()) {
    int value = camera_settings.hue();
    camera_->setCameraSettings(sl::CAMERA_SETTINGS_HUE, value);
  }

  if (camera_settings.has_gain()) {
    int value = camera_settings.gain();
    camera_->setCameraSettings(sl::CAMERA_SETTINGS_GAIN, value);
  }

  return Status::OK();
}

Status ZedCamera::GetCameraSettingsInfo(
    CameraSettingsInfoMessage* camera_settings) {
  if (camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }
  GetCameraSetting(sl::CAMERA_SETTINGS_AUTO_WHITEBALANCE,
                   camera_settings->mutable_white_balance_mode());
  GetCameraSetting(sl::CAMERA_SETTINGS_WHITEBALANCE,
                   camera_settings->mutable_color_temperature());
  GetCameraSetting(sl::CAMERA_SETTINGS_EXPOSURE,
                   camera_settings->mutable_exposure_time());
  GetCameraSetting(sl::CAMERA_SETTINGS_BRIGHTNESS,
                   camera_settings->mutable_brightness());
  GetCameraSetting(sl::CAMERA_SETTINGS_CONTRAST,
                   camera_settings->mutable_contrast());
  GetCameraSetting(sl::CAMERA_SETTINGS_SATURATION,
                   camera_settings->mutable_saturation());
  GetCameraSetting(sl::CAMERA_SETTINGS_HUE, camera_settings->mutable_hue());
  GetCameraSetting(sl::CAMERA_SETTINGS_GAIN, camera_settings->mutable_gain());
  return Status::OK();
}

namespace {

CameraSettingsMode ValueToMode(int value) {
  return value == 1 ? CameraSettingsMode::CAMERA_SETTINGS_MODE_AUTO
                    : CameraSettingsMode::CAMERA_SETTINGS_MODE_MANUAL;
}

}  // namespace

void ZedCamera::GetCameraSetting(sl::CAMERA_SETTINGS camera_setting,
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

void ZedCamera::GetCameraSetting(sl::CAMERA_SETTINGS camera_setting,
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
  {
    base::AutoLock l(lock_);
    if (is_stopping_) return;
  }

  sl::ERROR_CODE err = camera_->grab(runtime_params_);
  if (err != sl::SUCCESS && err != sl::ERROR_CODE_NOT_A_NEW_FRAME) {
    status_callback_.Run(felicia::errors::Unavailable(
        MESSAGE_WITH_ERROR_CODE("Failed to grab", err)));
    return;
  }

  base::TimeDelta timestamp = base::TimeDelta::FromNanoseconds(
      camera_->getTimestamp(sl::TIME_REFERENCE_CURRENT));

  if (!left_camera_frame_callback_.is_null()) {
    sl::Mat image;
    camera_->retrieveImage(image, sl::VIEW_LEFT);
    CameraFrame camera_frame = ConvertToCameraFrame(image, timestamp);
    left_camera_frame_callback_.Run(std::move(camera_frame));
  }
  if (!right_camera_frame_callback_.is_null()) {
    sl::Mat image;
    camera_->retrieveImage(image, sl::VIEW_RIGHT);
    CameraFrame camera_frame = ConvertToCameraFrame(image, timestamp);
    right_camera_frame_callback_.Run(std::move(camera_frame));
  }
  if (!depth_camera_frame_callback_.is_null()) {
    sl::Mat image;
    camera_->retrieveMeasure(image, sl::MEASURE_DEPTH);
    float min = camera_->getDepthMinRangeValue();
    float max = camera_->getDepthMaxRangeValue();
    DepthCameraFrame depth_camera_frame =
        ConvertToDepthCameraFrame(image, timestamp, min, max);
    depth_camera_frame_callback_.Run(std::move(depth_camera_frame));
  }
  if (!pointcloud_callback_.is_null()) {
    base::TimeDelta delta = timestamp - last_timestamp_;
    if (delta > base::TimeDelta::FromSeconds(1)) {
      sl::Mat cloud;
      camera_->retrieveMeasure(cloud, sl::MEASURE_XYZRGBA);
      map::Pointcloud pointcloud = ConvertToPointcloud(cloud, timestamp);
      pointcloud_callback_.Run(std::move(pointcloud));
      last_timestamp_ = timestamp;
    }
  }

  thread_.task_runner()->PostTask(
      FROM_HERE, base::BindOnce(&ZedCamera::DoGrab, AsWeakPtr()));
}

void ZedCamera::DoStop(base::WaitableEvent* event, Status* status) {
  DCHECK(thread_.task_runner()->BelongsToCurrentThread());

  camera_->close();

  *status = Status::OK();
}

// static
Status ZedCamera::OpenCamera(int device_id, sl::InitParameters& params,
                             ZedCamera::ScopedCamera* camera) {
  if (device_id == ZedCameraDescriptor::kInvalidId) {
    return felicia::errors::InvalidArgument(
        "No avaiable camera from given camera_descriptor.");
  } else {
    params.input.setFromCameraID(device_id);
  }

  sl::ERROR_CODE err = camera->get()->open(params);
  if (err != sl::SUCCESS) {
    return felicia::errors::Unavailable(
        MESSAGE_WITH_ERROR_CODE("Failed to open camera", err));
  }
  return Status::OK();
}

CameraFrame ZedCamera::ConvertToCameraFrame(sl::Mat image,
                                            base::TimeDelta timestamp) {
  size_t size = image.getStepBytes() * image.getHeight();
  Data data;
  data.resize(size);
  sl::MAT_TYPE data_type = image.getDataType();

  switch (data_type) {
    case sl::MAT_TYPE_32F_C1:
      memcpy(data.cast<char*>(), image.getPtr<sl::float1>(), size);
      break;
    case sl::MAT_TYPE_32F_C2:
      memcpy(data.cast<char*>(), image.getPtr<sl::float2>(), size);
      break;
    case sl::MAT_TYPE_32F_C3:
      memcpy(data.cast<char*>(), image.getPtr<sl::float3>(), size);
      break;
    case sl::MAT_TYPE_32F_C4:
      memcpy(data.cast<char*>(), image.getPtr<sl::float4>(), size);
      break;
    case sl::MAT_TYPE_8U_C1:
      memcpy(data.cast<char*>(), image.getPtr<sl::uchar1>(), size);
      break;
    case sl::MAT_TYPE_8U_C2:
      memcpy(data.cast<char*>(), image.getPtr<sl::uchar2>(), size);
      break;
    case sl::MAT_TYPE_8U_C3:
      memcpy(data.cast<char*>(), image.getPtr<sl::uchar3>(), size);
      break;
    case sl::MAT_TYPE_8U_C4:
      memcpy(data.cast<char*>(), image.getPtr<sl::uchar4>(), size);
      break;
  }

  return CameraFrame(std::move(data), camera_format_, timestamp);
}

DepthCameraFrame ZedCamera::ConvertToDepthCameraFrame(sl::Mat image,
                                                      base::TimeDelta timestamp,
                                                      float min, float max) {
  size_t size = image.getWidth() * image.getHeight();
  size_t allocation_size = 2 * size;
  Data data;
  data.resize(allocation_size);
  sl::float1* image_ptr = image.getPtr<sl::float1>();
  uint8_t* data_ptr = data.cast<uint8_t*>();
  if (init_params_.coordinate_units == sl::UNIT_MILLIMETER) {
    for (size_t i = 0; i < size; ++i) {
      const size_t data_idx = i << 1;
      uint16_t value = static_cast<int64_t>(*(image_ptr++));
      data_ptr[data_idx] = static_cast<uint8_t>(value & UINT8_MAX);
      data_ptr[data_idx + 1] = static_cast<uint8_t>(value >> 8);
    }
  } else {
    for (size_t i = 0; i < size; ++i) {
      const size_t data_idx = i << 1;
      uint16_t value =
          InMillimeter(init_params_.coordinate_units, *(image_ptr++));
      data_ptr[data_idx] = static_cast<uint8_t>(value & UINT8_MAX);
      data_ptr[data_idx + 1] = static_cast<uint8_t>(value >> 8);
    }
  }
  CameraFrame frame(std::move(data), depth_camera_format_, timestamp);
  return DepthCameraFrame(std::move(frame), min, max);
}

map::Pointcloud ZedCamera::ConvertToPointcloud(sl::Mat cloud,
                                               base::TimeDelta timestamp) {
  const float* cloud_ptr = cloud.getPtr<sl::float4>()->ptr();
  size_t size = cloud.getWidth() * cloud.getHeight();
  map::Pointcloud frame;
  struct Color8_4 {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  };

  frame.points().set_type(DATA_TYPE_32F_C3);
  frame.colors().set_type(DATA_TYPE_8U_C3);
  Data::View<Point3f> points = frame.points().AsView<Point3f>();
  Data::View<Color3u> colors = frame.colors().AsView<Color3u>();
  points.resize(size);
  colors.resize(size);
  if (init_params_.coordinate_units == sl::UNIT_METER &&
      init_params_.coordinate_system ==
          sl::COORDINATE_SYSTEM_LEFT_HANDED_Y_UP) {
    for (size_t i = 0; i < size; ++i) {
      const size_t cloud_ptr_idx = i << 2;
      Color8_4 c = bit_cast<Color8_4>(cloud_ptr[cloud_ptr_idx + 3]);
      points[i] =
          Point3f(cloud_ptr[cloud_ptr_idx], cloud_ptr[cloud_ptr_idx + 1],
                  cloud_ptr[cloud_ptr_idx + 2]);
      colors[i] = Color3u(c.r, c.g, c.b);
    }
  } else {
    for (size_t i = 0; i < size; ++i) {
      const size_t cloud_ptr_idx = i << 2;
      float x =
          InMeter(init_params_.coordinate_units, cloud_ptr[cloud_ptr_idx]);
      float y =
          InMeter(init_params_.coordinate_units, cloud_ptr[cloud_ptr_idx + 1]);
      float z =
          InMeter(init_params_.coordinate_units, cloud_ptr[cloud_ptr_idx + 2]);
      points[i] = coordinate_.Convert(
          Point3f(x, y, z), Coordinate::COORDINATE_SYSTEM_LEFT_HANDED_Y_UP);
      Color8_4 c = bit_cast<Color8_4>(cloud_ptr[cloud_ptr_idx + 3]);
      colors[i] = Color3u(c.r, c.g, c.b);
    }
  }
  frame.set_timestamp(timestamp);

  return frame;
}

#undef MESSAGE_WITH_ERROR_CODE

}  // namespace drivers
}  // namespace felicia