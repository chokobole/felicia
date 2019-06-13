#include "felicia/drivers/vendors/realsense/rs_camera.h"

#include "third_party/chromium/base/strings/strcat.h"

#include "felicia/drivers/camera/camera_errors.h"
#include "felicia/drivers/imu/imu_errors.h"
#include "felicia/drivers/vendors/realsense/rs_pixel_format.h"

namespace felicia {

namespace errors {

Status NotSupportedOption(rs2_option option) {
  return errors::NotFound(::base::StringPrintf("%s is not supported",
                                               rs2_option_to_string(option)));
}

}  // namespace errors

RsCamera::RsCamera(const CameraDescriptor& camera_descriptor)
    : DepthCameraInterface(camera_descriptor) {}

RsCamera::~RsCamera() = default;

Status RsCamera::Init() {
  if (!camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }

  Status s = RsCamera::CreateDevice(camera_descriptor_, &device_);
  if (!s.ok()) return s;

  std::vector<::rs2::sensor> sensors = device_.query_sensors();
  for (auto&& sensor : sensors) {
    std::string module_name = sensor.get_info(RS2_CAMERA_INFO_NAME);
    if ("Stereo Module" == module_name) {
      sensors_[RS_DEPTH] = sensor;
      sensors_[RS_INFRA1] = sensor;
      sensors_[RS_INFRA2] = sensor;

      depth_scale_ = sensor.as<::rs2::depth_sensor>().get_depth_scale();
    } else if ("Coded-Light Depth Sensor" == module_name) {
      sensors_[RS_DEPTH] = sensor;
      sensors_[RS_INFRA1] = sensor;

      depth_scale_ = sensor.as<::rs2::depth_sensor>().get_depth_scale();
      LOG(ERROR) << "Not Implemented yet for module : " << module_name;
    } else if ("RGB Camera" == module_name) {
      sensors_[RS_COLOR] = sensor;
    } else if ("Wide FOV Camera" == module_name) {
      sensors_[RS_FISHEYE] = sensor;
      LOG(ERROR) << "Not Implemented yet for module : " << module_name;
    } else if ("Motion Module" == module_name) {
      sensors_[RS_GYRO] = sensor;
      sensors_[RS_ACCEL] = sensor;
    } else if ("Tracking Module" == module_name) {
      sensors_[RS_GYRO] = sensor;
      sensors_[RS_ACCEL] = sensor;
      sensors_[RS_POSE] = sensor;
      sensors_[RS_FISHEYE1] = sensor;
      sensors_[RS_FISHEYE2] = sensor;
      LOG(ERROR) << "Not Implemented yet for module : " << module_name;
    }
  }

  camera_state_.ToInitialized();

  return CreateCapabilityMap(device_, &capability_map_);
}

Status RsCamera::Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       CameraFrameCallback color_frame_callback,
                       DepthCameraFrameCallback depth_frame_callback,
                       StatusCallback status_callback) {
  Status s = Start(requested_color_format, requested_depth_format, ImuFormat{},
                   ImuFormat{}, false /* imu */, false /* synched */);
  if (!s.ok()) return s;

  color_frame_callback_ = color_frame_callback;
  depth_frame_callback_ = depth_frame_callback;
  status_callback_ = status_callback;

  camera_state_.ToStarted();

  return Status::OK();
}

Status RsCamera::Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       AlignDirection align_direction,
                       SynchedDepthCameraFrameCallback synched_frame_callback,
                       StatusCallback status_callback) {
  Status s = Start(requested_color_format, requested_depth_format, ImuFormat{},
                   ImuFormat{}, false /* imu */, true /* synched */);
  if (!s.ok()) return s;

  synched_frame_callback_ = synched_frame_callback;
  status_callback_ = status_callback;

  SetRsAlignFromDirection(align_direction);

  camera_state_.ToStarted();

  return Status::OK();
}

Status RsCamera::Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       const ImuFormat& requested_gyro_format,
                       const ImuFormat& requested_accel_format,
                       ImuFilterFactory::ImuFilterKind kind,
                       CameraFrameCallback color_frame_callback,
                       DepthCameraFrameCallback depth_frame_callback,
                       ImuFrameCallback imu_frame_callback,
                       StatusCallback status_callback) {
  Status s = Start(requested_color_format, requested_depth_format,
                   requested_gyro_format, requested_accel_format,
                   true /* imu */, false /* synched */);
  if (!s.ok()) return s;

  color_frame_callback_ = color_frame_callback;
  depth_frame_callback_ = depth_frame_callback;
  imu_frame_callback_ = imu_frame_callback;
  status_callback_ = status_callback;

  imu_filter_ = ImuFilterFactory::NewImuFilter(kind);

  camera_state_.ToStarted();

  return Status::OK();
}

Status RsCamera::Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       AlignDirection align_direction,
                       const ImuFormat& requested_gyro_format,
                       const ImuFormat& requested_accel_format,
                       ImuFilterFactory::ImuFilterKind kind,
                       SynchedDepthCameraFrameCallback synched_frame_callback,
                       ImuFrameCallback imu_frame_callback,
                       StatusCallback status_callback) {
  Status s = Start(requested_color_format, requested_depth_format,
                   requested_gyro_format, requested_accel_format,
                   true /* imu */, true /* synched */);
  if (!s.ok()) return s;

  synched_frame_callback_ = synched_frame_callback;
  imu_frame_callback_ = imu_frame_callback;
  status_callback_ = status_callback;

  SetRsAlignFromDirection(align_direction);
  imu_filter_ = ImuFilterFactory::NewImuFilter(kind);

  camera_state_.ToStarted();

  return Status::OK();
}

Status RsCamera::Start(const CameraFormat& requested_color_format,
                       const CameraFormat& requested_depth_format,
                       const ImuFormat& requested_gyro_format,
                       const ImuFormat& requested_accel_format, bool imu,
                       bool synched) {
  if (!camera_state_.IsInitialized()) {
    return camera_state_.InvalidStateError();
  }

  std::function<void(::rs2::frame)> frame_callback_function;
  std::function<void(::rs2::frame)> imu_callback_function;

  if (synched) {
    frame_callback_function = syncer_;
    auto frame_callback_inner = [this](::rs2::frame frame) { OnFrame(frame); };
    syncer_.start(frame_callback_inner);
  } else {
    frame_callback_function = [this](rs2::frame frame) { OnFrame(frame); };
  }

  if (imu) {
    imu_callback_function = [this](::rs2::frame frame) { OnImuFrame(frame); };
  }

  bool imu_started = false;
  for (auto& sensor : sensors_) {
    try {
      if (sensor.first == RS_COLOR) {
        const RsCapability* found_capability = GetBestMatchedCapability(
            requested_color_format, capability_map_[sensor.first]);
        if (!found_capability) return errors::NoVideoCapbility();
        color_format_ = found_capability->format.camera_format;
        if (requested_color_format.convert_to_argb()) {
          color_format_.set_convert_to_argb(true);
        }
        sensor.second.open(
            sensor.second
                .get_stream_profiles()[found_capability->stream_index]);
        sensor.second.start(frame_callback_function);
      } else if (sensor.first == RS_DEPTH) {
        const RsCapability* found_capability = GetBestMatchedCapability(
            requested_depth_format, capability_map_[sensor.first]);
        if (!found_capability) return errors::NoVideoCapbility();
        depth_format_ = found_capability->format.camera_format;
        sensor.second.open(
            sensor.second
                .get_stream_profiles()[found_capability->stream_index]);
        sensor.second.start(frame_callback_function);
      } else if (sensor.first == RS_GYRO || sensor.first == RS_ACCEL) {
        if (!imu) continue;
        if (imu_started) continue;
        imu_started = true;
        const RsCapability* found_gyro_capability = GetBestMatchedCapability(
            requested_gyro_format, capability_map_[RS_GYRO]);
        if (!found_gyro_capability) return errors::NoImuCapability();
        gyro_format_ = found_gyro_capability->format.imu_format;
        const RsCapability* found_accel_capability = GetBestMatchedCapability(
            requested_accel_format, capability_map_[RS_ACCEL]);
        if (!found_accel_capability) return errors::NoImuCapability();
        accel_format_ = found_accel_capability->format.imu_format;
        sensor.second.open(
            {sensor.second
                 .get_stream_profiles()[found_accel_capability->stream_index],
             sensor.second
                 .get_stream_profiles()[found_gyro_capability->stream_index]});
        sensor.second.start(imu_callback_function);
      }
    } catch (::rs2::error e) {
      return Status(error::UNAVAILABLE, e.what());
    }
  }

  return Status::OK();
}

Status RsCamera::Stop() {
  if (!camera_state_.IsStarted()) {
    return camera_state_.InvalidStateError();
  }

  for (auto& sensor : sensors_) {
    if (sensor.first == RS_COLOR || sensor.first == RS_DEPTH) {
      try {
        sensor.second.stop();
        sensor.second.close();
      } catch (::rs2::error e) {
        return Status(error::UNAVAILABLE, e.what());
      }
    }
  }

  color_frame_callback_.Reset();
  depth_frame_callback_.Reset();
  synched_frame_callback_.Reset();

  camera_state_.ToStopped();

  return Status::OK();
}

StatusOr<::rs2::sensor> RsCamera::sensor(const RsStreamInfo& rs_stream_info) {
  auto it = sensors_.find(rs_stream_info);
  if (it == sensors_.end()) {
    return errors::NotFound("No sensor");
  }
  return it->second;
}

Status RsCamera::SetCameraSettings(const CameraSettings& camera_settings) {
  if (camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }

  auto status_or = sensor(RS_COLOR);
  if (!status_or.ok()) return status_or.status();
  ::rs2::sensor& s = status_or.ValueOrDie();

  if (camera_settings.has_white_balance_mode()) {
    const float value =
        camera_settings.white_balance_mode() == CAMERA_SETTINGS_MODE_AUTO ? 1
                                                                          : 0;
    SetOption(s, RS2_OPTION_ENABLE_AUTO_WHITE_BALANCE, value);
  }

  if (camera_settings.has_color_temperature()) {
    bool can_set = false;
    {
      float value;
      if (GetOption(s, RS2_OPTION_ENABLE_AUTO_WHITE_BALANCE, &value).ok()) {
        can_set = value == 1;
      }
    }
    if (can_set) {
      const float value = camera_settings.color_temperature();
      SetOption(s, RS2_OPTION_WHITE_BALANCE, value);
    }
  }

  if (camera_settings.has_exposure_mode()) {
    const float value =
        camera_settings.exposure_mode() == CAMERA_SETTINGS_MODE_AUTO ? 1 : 0;
    SetOption(s, RS2_OPTION_ENABLE_AUTO_EXPOSURE, value);
  }

  if (camera_settings.has_exposure_time()) {
    bool can_set = false;
    {
      float value;
      if (GetOption(s, RS2_OPTION_ENABLE_AUTO_EXPOSURE, &value).ok()) {
        can_set = value == 1;
      }
    }
    if (can_set) {
      const float value = camera_settings.exposure_time();
      SetOption(s, RS2_OPTION_EXPOSURE, value);
    }
  }

  if (camera_settings.has_brightness()) {
    const float value = camera_settings.brightness();
    SetOption(s, RS2_OPTION_BRIGHTNESS, value);
  }

  if (camera_settings.has_contrast()) {
    const float value = camera_settings.contrast();
    SetOption(s, RS2_OPTION_CONTRAST, value);
  }

  if (camera_settings.has_saturation()) {
    const float value = camera_settings.saturation();
    SetOption(s, RS2_OPTION_SATURATION, value);
  }

  if (camera_settings.has_sharpness()) {
    const float value = camera_settings.sharpness();
    SetOption(s, RS2_OPTION_SHARPNESS, value);
  }

  if (camera_settings.has_hue()) {
    const float value = camera_settings.hue();
    SetOption(s, RS2_OPTION_HUE, value);
  }

  if (camera_settings.has_gain()) {
    const float value = camera_settings.gain();
    SetOption(s, RS2_OPTION_GAIN, value);
  }

  if (camera_settings.has_gamma()) {
    const float value = camera_settings.gamma();
    SetOption(s, RS2_OPTION_GAMMA, value);
  }

  return Status::OK();
}

Status RsCamera::SetOption(::rs2::sensor& sensor, rs2_option option,
                           float value) {
  if (!sensor.supports(option)) {
    return errors::NotSupportedOption(option);
  }

  try {
    sensor.set_option(option, value);
    return Status::OK();
  } catch (const ::rs2::error& e) {
    return errors::Unavailable(
        ::base::StringPrintf("Failed to set_option(%s): %s.",
                             rs2_option_to_string(option), e.what()));
  }

  return errors::Internal("Not reached");
}

Status RsCamera::GetOption(::rs2::sensor& sensor, rs2_option option,
                           float* value) {
  if (!sensor.supports(option)) {
    return errors::NotSupportedOption(option);
  }

  try {
    *value = sensor.get_option(option);
    return Status::OK();
  } catch (const ::rs2::error& e) {
    return errors::Unavailable(
        ::base::StringPrintf("Failed to get_option(%s): %s.",
                             rs2_option_to_string(option), e.what()));
  }

  return errors::Internal("Not reached");
}

Status RsCamera::GetOptionRange(::rs2::sensor& sensor, rs2_option option,
                                ::rs2::option_range* option_range) {
  if (!sensor.supports(option)) {
    return errors::NotSupportedOption(option);
  }

  try {
    *option_range = sensor.get_option_range(option);
    return Status::OK();
  } catch (const ::rs2::error& e) {
    return errors::Unavailable(
        ::base::StringPrintf("Failed to get_option_range(%s): %s.",
                             rs2_option_to_string(option), e.what()));
  }

  return errors::Internal("Not reached");
}

Status RsCamera::GetCameraSettingsInfo(
    CameraSettingsInfoMessage* camera_settings) {
  if (camera_state_.IsStopped()) {
    return camera_state_.InvalidStateError();
  }

  auto status_or = sensor(RS_COLOR);
  if (!status_or.ok()) return status_or.status();
  ::rs2::sensor& s = status_or.ValueOrDie();

  GetCameraSetting(s, RS2_OPTION_ENABLE_AUTO_WHITE_BALANCE,
                   camera_settings->mutable_white_balance_mode());
  GetCameraSetting(s, RS2_OPTION_ENABLE_AUTO_EXPOSURE,
                   camera_settings->mutable_exposure_mode());
  GetCameraSetting(s, RS2_OPTION_EXPOSURE,
                   camera_settings->mutable_exposure_time());
  GetCameraSetting(s, RS2_OPTION_WHITE_BALANCE,
                   camera_settings->mutable_color_temperature());
  GetCameraSetting(s, RS2_OPTION_BRIGHTNESS,
                   camera_settings->mutable_brightness());
  GetCameraSetting(s, RS2_OPTION_CONTRAST, camera_settings->mutable_contrast());
  GetCameraSetting(s, RS2_OPTION_SATURATION,
                   camera_settings->mutable_saturation());
  GetCameraSetting(s, RS2_OPTION_SHARPNESS,
                   camera_settings->mutable_sharpness());
  GetCameraSetting(s, RS2_OPTION_HUE, camera_settings->mutable_hue());
  GetCameraSetting(s, RS2_OPTION_GAIN, camera_settings->mutable_gain());
  GetCameraSetting(s, RS2_OPTION_GAMMA, camera_settings->mutable_gamma());
  return Status::OK();
}

Status RsCamera::GetAllOptions(::rs2::sensor& sensor,
                               std::vector<rs2_option>* options) {
  DCHECK(options->empty());
  for (int i = 0; i < static_cast<int>(RS2_OPTION_COUNT); ++i) {
    rs2_option option = static_cast<rs2_option>(i);
    if (sensor.supports(option)) {
      options->push_back(option);
    }
  }
  return Status::OK();
}

namespace {

CameraSettingsMode ValueToMode(float value) {
  return value ? CameraSettingsMode::CAMERA_SETTINGS_MODE_AUTO
               : CameraSettingsMode::CAMERA_SETTINGS_MODE_MANUAL;
}

}  // namespace

void RsCamera::GetCameraSetting(::rs2::sensor& sensor, rs2_option option,
                                CameraSettingsModeValue* value) {
  ::rs2::option_range option_range;
  if (!GetOptionRange(sensor, option, &option_range).ok()) {
    value->Clear();
    return;
  }
  value->add_modes(CameraSettingsMode::CAMERA_SETTINGS_MODE_AUTO);
  value->add_modes(CameraSettingsMode::CAMERA_SETTINGS_MODE_MANUAL);
  value->set_default_(ValueToMode(option_range.def));

  float v;
  if (!GetOption(sensor, option, &v).ok()) {
    value->Clear();
    return;
  }
  value->set_current(ValueToMode(v));
}

void RsCamera::GetCameraSetting(::rs2::sensor& sensor, rs2_option option,
                                CameraSettingsRangedValue* value) {
  ::rs2::option_range option_range;
  if (!GetOptionRange(sensor, option, &option_range).ok()) {
    value->Clear();
    return;
  }
  value->set_min(static_cast<int64_t>(option_range.min));
  value->set_max(static_cast<int64_t>(option_range.max));
  value->set_step(static_cast<int64_t>(option_range.step));
  value->set_default_(static_cast<int64_t>(option_range.def));

  float v;
  if (!GetOption(sensor, option, &v).ok()) {
    value->Clear();
    return;
  }
  value->set_current(static_cast<int64_t>(v));
}

void RsCamera::SetRsAlignFromDirection(AlignDirection align_direction) {
  if (align_direction == AlignDirection::AlignToColor) {
    align_ = std::make_unique<::rs2::align>(RS2_STREAM_COLOR);
  } else if (align_direction == AlignDirection::AlignToDepth) {
    align_ = std::make_unique<::rs2::align>(RS2_STREAM_DEPTH);
  }
}

void RsCamera::OnFrame(::rs2::frame frame) {
  if (frame.is<::rs2::frameset>()) {
    auto frameset = frame.as<::rs2::frameset>();
    if (align_) {
      frameset = align_->process(frameset);
    }

    ::rs2::video_frame rs_color_frame = frameset.get_color_frame();
    ::rs2::depth_frame rs_depth_frame = frameset.get_depth_frame();

    DepthCameraFrame depth_frame = FromRsDepthFrame(rs_depth_frame);
    if (color_format_.convert_to_argb()) {
      auto color_frame = ConvertToARGB(rs_color_frame);
      if (color_frame.has_value()) {
        synched_frame_callback_.Run(std::move(color_frame.value()),
                                    std::move(depth_frame));
      } else {
        status_callback_.Run(errors::FailedToConvertToARGB());
      }
    } else {
      CameraFrame color_frame = FromRsColorFrame(rs_color_frame);
      synched_frame_callback_.Run(std::move(color_frame),
                                  std::move(depth_frame));
    }
  } else if (frame.is<::rs2::video_frame>()) {
    if (frame.is<::rs2::depth_frame>()) {
      depth_frame_callback_.Run(
          FromRsDepthFrame(frame.as<::rs2::depth_frame>()));
    } else {
      if (color_format_.convert_to_argb()) {
        auto color_frame = ConvertToARGB(frame.as<::rs2::video_frame>());
        if (color_frame.has_value()) {
          color_frame_callback_.Run(std::move(color_frame.value()));
        } else {
          status_callback_.Run(errors::FailedToConvertToARGB());
        }
      } else {
        color_frame_callback_.Run(
            FromRsColorFrame(frame.as<::rs2::video_frame>()));
      }
    }
  }
}

void RsCamera::OnImuFrame(::rs2::frame frame) {
  auto motion = frame.as<rs2::motion_frame>();
  auto stream = frame.get_profile().stream_type();
  ImuFrame imu_frame;

  rs2_vector vector = motion.get_motion_data();
  ::base::TimeDelta timestamp = timestamper_.timestamp();
  if (stream == RS_GYRO.stream_type) {
    imu_frame.set_angulary_veilocity(vector.x, vector.y, vector.z);
    imu_filter_->UpdateAngularVelocity(vector.x, vector.y, vector.z, timestamp);
  } else {
    imu_frame.set_linear_acceleration(vector.x, vector.y, vector.z);
    imu_filter_->UpdateLinearAcceleration(vector.x, vector.y, vector.z);
  }
  imu_frame.set_timestamp(timestamp);
  imu_frame.set_orientation(imu_filter_->orientation());

  imu_frame_callback_.Run(imu_frame);
}

::base::Optional<CameraFrame> RsCamera::ConvertToARGB(
    ::rs2::video_frame color_frame) {
  size_t length = color_format_.AllocationSize();
  CameraBuffer camera_buffer(
      reinterpret_cast<uint8_t*>(const_cast<void*>(color_frame.get_data())),
      length);
  camera_buffer.set_payload(length);
  ::base::Optional<CameraFrame> argb_frame =
      felicia::ConvertToARGB(camera_buffer, color_format_);
  if (argb_frame.has_value()) {
    argb_frame.value().set_timestamp(timestamper_.timestamp());
  }

  return argb_frame;
}

CameraFrame RsCamera::FromRsColorFrame(::rs2::video_frame color_frame) {
  size_t length = color_format_.AllocationSize();
  std::unique_ptr<uint8_t[]> new_color_frame(new uint8_t[length]);
  memcpy(new_color_frame.get(), color_frame.get_data(), length);
  CameraFrame camera_frame(std::move(new_color_frame), length, color_format_);
  camera_frame.set_timestamp(timestamper_.timestamp());
  return camera_frame;
}

DepthCameraFrame RsCamera::FromRsDepthFrame(::rs2::depth_frame depth_frame) {
  size_t length = depth_format_.AllocationSize();
  std::unique_ptr<uint8_t[]> new_depth_frame(new uint8_t[length]);
  memcpy(new_depth_frame.get(), depth_frame.get_data(), length);
  DepthCameraFrame depth_camera_frame(std::move(new_depth_frame), length,
                                      depth_format_, depth_scale_);
  depth_camera_frame.set_timestamp(timestamper_.timestamp());
  return depth_camera_frame;
}

// static
Status RsCamera::CreateDevice(const CameraDescriptor& camera_descriptor,
                              ::rs2::device* device) {
  ::rs2::context context;
  auto list = context.query_devices();
  for (auto&& dev : list) {
    const char* physical_port = dev.get_info(RS2_CAMERA_INFO_PHYSICAL_PORT);
    if (physical_port == camera_descriptor.device_id()) {
      *device = dev;
      return Status::OK();
    }
  }

  return Status(error::NOT_FOUND,
                ::base::StrCat({"No mathcing device with ",
                                camera_descriptor.ToString()}));
}

// static
Status RsCamera::CreateCapabilityMap(::rs2::device device,
                                     RsCapabilityMap* rs_capability_map) {
  std::vector<::rs2::sensor> sensors = device.query_sensors();
  for (auto&& sensor : sensors) {
    std::vector<::rs2::stream_profile> profiles = sensor.get_stream_profiles();
    for (size_t i = 0; i < profiles.size(); ++i) {
      ::rs2::stream_profile profile = profiles[i];
      if (profile.is<::rs2::video_stream_profile>()) {
        auto video_profile = profile.as<::rs2::video_stream_profile>();

        if (video_profile.stream_type() == RS2_STREAM_INFRARED ||
            video_profile.stream_type() == RS2_STREAM_FISHEYE) {
          LOG(ERROR) << "Not supported yet for the stream type : "
                     << rs2_stream_to_string(video_profile.stream_type());
          continue;
        }

        RsStreamInfo steram_info{video_profile.stream_type(),
                                 video_profile.stream_index()};
        if (rs_capability_map->find(steram_info) == rs_capability_map->end()) {
          (*rs_capability_map)[steram_info] = RsCapabilityList{};
        }

        (*rs_capability_map)[steram_info].emplace_back(
            i, CameraFormat{video_profile.width(), video_profile.height(),
                            FromRs2Format(video_profile.format()),
                            static_cast<float>(video_profile.fps())});
      } else if (profile.is<::rs2::motion_stream_profile>()) {
        auto motion_profile = profile.as<::rs2::motion_stream_profile>();

        if (motion_profile.stream_type() == RS2_STREAM_POSE) {
          LOG(ERROR) << "Not supported yet for the stream type : "
                     << rs2_stream_to_string(motion_profile.stream_type());
          continue;
        }

        RsStreamInfo steram_info{motion_profile.stream_type(),
                                 motion_profile.stream_index()};
        if (rs_capability_map->find(steram_info) == rs_capability_map->end()) {
          (*rs_capability_map)[steram_info] = RsCapabilityList{};
        }

        (*rs_capability_map)[steram_info].emplace_back(
            i, ImuFormat{static_cast<float>(motion_profile.fps())});
      }
    }
  }

  if ((*rs_capability_map)[RS_COLOR].empty() ||
      (*rs_capability_map)[RS_DEPTH].empty()) {
    return errors::NoVideoCapbility();
  }

  return Status::OK();
}

}  // namespace felicia