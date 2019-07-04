// Some of implementations are taken and modified from
// https://github.com/IntelRealSense/realsense-ros/blob/development/realsense2_camera/src/base_realsense_node.cpp

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
  StartParams params;
  params.requested_color_format = requested_color_format;
  params.requested_depth_format = requested_depth_format;
  params.color_frame_callback = color_frame_callback;
  params.depth_frame_callback = depth_frame_callback;
  return Start(params);
}

Status RsCamera::Start(const RsCamera::StartParams& params) {
  if (!camera_state_.IsInitialized()) {
    return camera_state_.InvalidStateError();
  }

  bool has_color_callback = !params.color_frame_callback.is_null();
  bool has_depth_callback = !params.depth_frame_callback.is_null();
  bool has_pointcloud_callback = !params.pointcloud_frame_callback.is_null();
  bool has_imu_callback = !params.imu_frame_callback.is_null();

  if (params.status_callback.is_null()) {
    return errors::InvalidArgument("status_callback is null.");
  }

  if (!(has_color_callback || has_depth_callback || has_pointcloud_callback ||
        has_imu_callback)) {
    return errors::InvalidArgument("There's no callback to receive frame.");
  }

  std::function<void(::rs2::frame)> frame_callback_function;
  std::function<void(::rs2::frame)> imu_callback_function;

  if (params.named_filters.size() > 0) {
    frame_callback_function = syncer_;
    auto frame_callback_inner = [this](::rs2::frame frame) { OnFrame(frame); };
    syncer_.start(frame_callback_inner);
  } else if (has_color_callback || has_depth_callback) {
    frame_callback_function = [this](rs2::frame frame) { OnFrame(frame); };
  }

  if (has_imu_callback) {
    imu_callback_function = [this](::rs2::frame frame) { OnImuFrame(frame); };
  }

  bool imu_started = false;
  for (auto& sensor : sensors_) {
    try {
      if (sensor.first == RS_COLOR) {
        const RsCapability* found_capability = GetBestMatchedCapability(
            params.requested_color_format, capability_map_[sensor.first]);
        if (!found_capability) return errors::NoVideoCapbility();
        color_format_ = found_capability->format.camera_format;
        requested_pixel_format_ = params.requested_color_format.pixel_format();
        sensor.second.open(
            sensor.second
                .get_stream_profiles()[found_capability->stream_index]);
        sensor.second.start(frame_callback_function);
      } else if (sensor.first == RS_DEPTH) {
        const RsCapability* found_capability = GetBestMatchedCapability(
            params.requested_depth_format, capability_map_[sensor.first]);
        if (!found_capability) return errors::NoVideoCapbility();
        depth_format_ = found_capability->format.camera_format;
        sensor.second.open(
            sensor.second
                .get_stream_profiles()[found_capability->stream_index]);
        sensor.second.start(frame_callback_function);
      } else if (sensor.first == RS_GYRO || sensor.first == RS_ACCEL) {
        if (!has_imu_callback) continue;
        if (imu_started) continue;
        imu_started = true;
        const RsCapability* found_gyro_capability = GetBestMatchedCapability(
            params.requested_gyro_format, capability_map_[RS_GYRO]);
        if (!found_gyro_capability) return errors::NoImuCapability();
        gyro_format_ = found_gyro_capability->format.imu_format;
        const RsCapability* found_accel_capability = GetBestMatchedCapability(
            params.requested_accel_format, capability_map_[RS_ACCEL]);
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

  if (has_color_callback) {
    color_frame_callback_ = params.color_frame_callback;
  }
  if (has_depth_callback) {
    depth_frame_callback_ = params.depth_frame_callback;
  }
  if (has_pointcloud_callback) {
    pointcloud_frame_callback_ = params.pointcloud_frame_callback;
  }
  if (has_imu_callback) {
    imu_filter_ = ImuFilterFactory::NewImuFilter(params.imu_filter_kind);
    imu_frame_callback_ = params.imu_frame_callback;
  }
  status_callback_ = params.status_callback;

  named_filters_ = params.named_filters;

  camera_state_.ToStarted();

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
  imu_frame_callback_.Reset();
  pointcloud_frame_callback_.Reset();
  status_callback_.Reset();

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

void RsCamera::OnFrame(::rs2::frame frame) {
  ::base::TimeDelta timestamp = timestamper_.timestamp();
  if (frame.is<::rs2::frameset>()) {
    auto frameset = frame.as<::rs2::frameset>();

    for (auto& named_filter : named_filters_) {
      frameset = named_filter.filter->process(frameset);
    }

    if (!pointcloud_frame_callback_.is_null()) {
      for (auto frame : frameset) {
        if (frame.is<::rs2::points>()) {
          HandlePoints(frame.as<::rs2::points>(), timestamp, frameset);
          break;
        }
      }
    }

    for (auto frame : frameset) {
      if (frame.is<::rs2::video_frame>()) {
        HandleVideoFrame(frame.as<::rs2::video_frame>(), timestamp);
      }
    }
  } else if (frame.is<::rs2::video_frame>()) {
    HandleVideoFrame(frame.as<::rs2::video_frame>(), timestamp);
  }
}

void RsCamera::OnImuFrame(::rs2::frame frame) {
  if (imu_frame_callback_.is_null()) return;

  auto motion = frame.as<rs2::motion_frame>();
  auto stream = frame.get_profile().stream_type();
  ImuFrame imu_frame;

  rs2_vector vector = motion.get_motion_data();
  Point3f point =
      coordinate_.Convert(Point3f(vector.x, vector.y, vector.z),
                          Coordinate::COORDINATE_SYSTEM_LEFT_HANDED_Y_UP);
  float x = point.x(), y = point.y(), z = point.z();
  ::base::TimeDelta timestamp = timestamper_.timestamp();
  if (stream == RS_GYRO.stream_type) {
    imu_frame.set_angulary_veilocity(x, y, z);
    imu_filter_->UpdateAngularVelocity(x, y, z, timestamp);
  } else {
    imu_frame.set_linear_acceleration(x, y, z);
    imu_filter_->UpdateLinearAcceleration(x, y, z);
  }
  imu_frame.set_timestamp(timestamp);
  imu_frame.set_orientation(imu_filter_->orientation());

  imu_frame_callback_.Run(imu_frame);
}

void RsCamera::HandleVideoFrame(::rs2::video_frame frame,
                                ::base::TimeDelta timestamp) {
  if (frame.is<::rs2::depth_frame>()) {
    if (depth_frame_callback_.is_null()) return;

    depth_frame_callback_.Run(
        FromRsDepthFrame(frame.as<::rs2::depth_frame>(), timestamp));
  } else {
    if (color_frame_callback_.is_null()) return;

    if (requested_pixel_format_ == color_format_.pixel_format()) {
      color_frame_callback_.Run(FromRsColorFrame(frame, timestamp));
    } else if (requested_pixel_format_ == cached_color_frame_.pixel_format() &&
               cached_color_frame_.data_ptr()) {
      color_frame_callback_.Run(std::move(cached_color_frame_));
    } else {
      ::base::Optional<CameraFrame> color_frame = ConvertToRequestedPixelFormat(
          frame, requested_pixel_format_, timestamp);
      if (color_frame.has_value()) {
        color_frame_callback_.Run(std::move(color_frame.value()));
      } else {
        status_callback_.Run(errors::FailedToConvertToRequestedPixelFormat(
            requested_pixel_format_));
      }
    }
  }
}

void RsCamera::HandlePoints(::rs2::points points, ::base::TimeDelta timestamp,
                            const ::rs2::frameset& frameset) {
  if (pointcloud_frame_callback_.is_null()) return;

  auto pc_filter_iter = std::find_if(
      named_filters_.begin(), named_filters_.end(),
      [](NamedFilter named_filter) {
        return named_filter.name == RsCamera::NamedFilter::POINTCLOUD;
      });
  if (pc_filter_iter == named_filters_.end()) return;

  rs2_stream texture_source_id = static_cast<rs2_stream>(
      pc_filter_iter->filter->get_option(rs2_option::RS2_OPTION_STREAM_FILTER));
  PointcloudFrame pointcloud_frame(points.size(), points.size());
  bool use_texture = texture_source_id != RS2_STREAM_ANY;
  rs2::frameset::iterator texture_frame_itr = frameset.end();
  if (use_texture) {
    int width, height, bpp;
    ColorIndexes color_indexes;
    const uint8_t* color = nullptr;
    if (texture_source_id == RS2_STREAM_COLOR) {
      PixelFormat final_pixel_format;
      if (requested_pixel_format_ == PIXEL_FORMAT_BGR) {
        bpp = 3;
        color_indexes = kBGR;
        final_pixel_format = requested_pixel_format_;
        cached_color_frame_ =
            FromRsColorFrame(frameset.get_color_frame(), timestamp);
      } else if (requested_pixel_format_ == PIXEL_FORMAT_BGRA) {
        bpp = 4;
        color_indexes = kBGRA;
        final_pixel_format = requested_pixel_format_;
        cached_color_frame_ =
            FromRsColorFrame(frameset.get_color_frame(), timestamp);
      } else if (requested_pixel_format_ == PIXEL_FORMAT_RGB) {
        bpp = 3;
        color_indexes = kRGB;
        final_pixel_format = requested_pixel_format_;
        cached_color_frame_ =
            FromRsColorFrame(frameset.get_color_frame(), timestamp);
      } else if (requested_pixel_format_ == PIXEL_FORMAT_ARGB) {
        bpp = 4;
        color_indexes = kARGB;
        final_pixel_format = requested_pixel_format_;
        cached_color_frame_ =
            FromRsColorFrame(frameset.get_color_frame(), timestamp);
      } else {
        bpp = 4;
        color_indexes = kBGRA;
        final_pixel_format = PIXEL_FORMAT_BGRA;
        auto color_frame = ConvertToRequestedPixelFormat(
            frameset.get_color_frame(), final_pixel_format, timestamp);
        if (color_frame.has_value()) {
          cached_color_frame_ = std::move(color_frame.value());
        } else {
          LOG(ERROR) << errors::FailedToConvertToRequestedPixelFormat(
              final_pixel_format);
          return;
        }
      }

      width = cached_color_frame_.width();
      height = cached_color_frame_.height();
      color = cached_color_frame_.data_ptr();
    } else {
      std::set<rs2_format> available_formats{rs2_format::RS2_FORMAT_RGB8,
                                             rs2_format::RS2_FORMAT_Y8};

      texture_frame_itr = std::find_if(
          frameset.begin(), frameset.end(),
          [&texture_source_id, &available_formats](rs2::frame f) {
            return (rs2_stream(f.get_profile().stream_type()) ==
                    texture_source_id) &&
                   (available_formats.find(f.get_profile().format()) !=
                    available_formats.end());
          });
      if (texture_frame_itr == frameset.end()) {
        std::string texture_source_name =
            pc_filter_iter->filter->get_option_value_description(
                rs2_option::RS2_OPTION_STREAM_FILTER,
                static_cast<float>(texture_source_id));
        LOG(WARNING) << "No stream match for pointcloud chosen texture "
                     << texture_source_name;
        return;
      }

      rs2::video_frame texture_frame =
          (*texture_frame_itr).as<rs2::video_frame>();
      width = texture_frame.get_width();
      height = texture_frame.get_height();
      bpp = texture_frame.get_bytes_per_pixel();
      color = reinterpret_cast<const uint8_t*>(texture_frame.get_data());
    }

    const rs2::vertex* vertex = points.get_vertices();
    const rs2::texture_coordinate* uv = points.get_texture_coordinates();
    for (size_t i = 0; i < points.size(); ++i) {
      float u = static_cast<float>(uv[i].u);
      float v = static_cast<float>(uv[i].v);
      if (u >= 0.f && u <= 1.f && v >= 0.f && v <= 1.f) {
        int x = static_cast<int>(u * width);
        int y = static_cast<int>(v * height);
        size_t offset = (y * width + x) * bpp;
        Point3f point =
            coordinate_.Convert(Point3f(vertex[i].x, vertex[i].y, vertex[i].z),
                                Coordinate::COORDINATE_SYSTEM_LEFT_HANDED_Y_UP);
        pointcloud_frame.AddPointAndColor(
            point.x(), point.y(), point.z(), color[offset + color_indexes.r],
            color[offset + color_indexes.g], color[offset + color_indexes.b]);
      }
    }
  } else {
    const rs2::vertex* vertex = points.get_vertices();
    for (size_t i = 0; i < points.size(); ++i) {
      if (vertex[i].z > 0) {
        Point3f point =
            coordinate_.Convert(Point3f(vertex[i].x, vertex[i].y, vertex[i].z),
                                Coordinate::COORDINATE_SYSTEM_LEFT_HANDED_Y_UP);
        pointcloud_frame.AddPoint(point.x(), point.y(), point.z());
      }
    }
  }
  pointcloud_frame.set_timestamp(timestamp);
  pointcloud_frame_callback_.Run(std::move(pointcloud_frame));
}

::base::Optional<CameraFrame> RsCamera::ConvertToRequestedPixelFormat(
    ::rs2::video_frame color_frame, PixelFormat requested_pixel_format,
    ::base::TimeDelta timestamp) {
  const uint8_t* data =
      reinterpret_cast<const uint8_t*>(color_frame.get_data());
  size_t length = color_format_.AllocationSize();
  return felicia::ConvertToRequestedPixelFormat(
      data, length, color_format_, requested_pixel_format, timestamp);
}

CameraFrame RsCamera::FromRsColorFrame(::rs2::video_frame color_frame,
                                       ::base::TimeDelta timestamp) {
  size_t length = color_format_.AllocationSize();
  std::unique_ptr<uint8_t[]> new_color_frame(new uint8_t[length]);
  memcpy(new_color_frame.get(), color_frame.get_data(), length);
  return CameraFrame{std::move(new_color_frame), length, color_format_,
                     timestamp};
}

DepthCameraFrame RsCamera::FromRsDepthFrame(::rs2::depth_frame depth_frame,
                                            ::base::TimeDelta timestamp) {
  size_t size = depth_format_.width() * depth_format_.height();
  size_t allocation_size = depth_format_.AllocationSize();
  std::unique_ptr<uint8_t[]> new_depth_frame(new uint8_t[allocation_size]);
  memcpy(new_depth_frame.get(), depth_frame.get_data(), allocation_size);
  uint16_t* ptr = reinterpret_cast<uint16_t*>(new_depth_frame.get());
  for (size_t i = 0; i < size; ++i) {
    const size_t data_idx = i << 1;
    uint16_t value = static_cast<uint16_t>(new_depth_frame[data_idx]) |
                     static_cast<uint16_t>(new_depth_frame[data_idx + 1] << 8);
    ptr[i] = static_cast<uint16_t>(
        std::round(value * depth_scale_ * 1000));  // in mm
  }
  CameraFrame camera_frame(std::move(new_depth_frame), allocation_size,
                           depth_format_, timestamp);
  DepthCameraFrame depth_camera_frame(std::move(camera_frame),
                                      105 /* 0.105 m */, UINT16_MAX);
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