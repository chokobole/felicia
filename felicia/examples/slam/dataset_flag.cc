#include "felicia/examples/slam/dataset_flag.h"

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/protobuf/channel.pb.h"
#include "felicia/slam/dataset/sensor_data.h"

namespace felicia {

static const char* kEuroc = "EUROC";
static const char* kKitti = "KITTI";
static const char* kTum = "TUM";

DatasetFlag::DatasetFlag() : current_dataset_kind_(DATASET_KIND_NONE) {
  {
    StringChoicesFlag::Builder builder(
        MakeValueStore(&dataset_kind_, std::string{kEuroc},
                       Choices<std::string>{kEuroc, kKitti, kTum}));
    auto flag = builder.SetName("dataset_kind")
                    .SetHelp("dataset kind to load (default: EUROC)")
                    .Build();
    dataset_kind_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&path_));
    auto flag = builder.SetName("path").SetHelp("path to dataset").Build();
    path_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&name_));
    auto flag = builder.SetShortName("-n")
                    .SetLongName("--name")
                    .SetHelp("name for node")
                    .Build();
    name_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringChoicesFlag::Builder builder(MakeValueStore<std::string>(
        &channel_type_, ChannelDef_Type_Name(ChannelDef::CHANNEL_TYPE_TCP),
        Choices<std::string>{
            ChannelDef_Type_Name(ChannelDef::CHANNEL_TYPE_TCP),
            ChannelDef_Type_Name(ChannelDef::CHANNEL_TYPE_SHM),
        }));
    auto flag =
        builder.SetShortName("-c")
            .SetLongName("--channel_type")
            .SetHelp(base::StringPrintf(
                "protocol to deliver message (default: %s)",
                ChannelDef_Type_Name(ChannelDef::CHANNEL_TYPE_TCP).c_str()))
            .Build();
    channel_type_flag_ = std::make_unique<StringChoicesFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&left_color_topic_));
    auto flag =
        builder.SetLongName("--left_color_topic")
            .SetHelp("topic to publish mono camera or left camera of stereo")
            .Build();
    left_color_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&right_color_topic_));
    auto flag = builder.SetLongName("--right_color_topic")
                    .SetHelp("topic to publish right camera of stereo")
                    .Build();
    right_color_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&depth_topic_));
    auto flag = builder.SetLongName("--depth_topic")
                    .SetHelp("topic to publish depth of depth camera")
                    .Build();
    depth_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&lidar_topic_));
    auto flag = builder.SetLongName("--lidar_topic")
                    .SetHelp("topic to publish lidar")
                    .Build();
    lidar_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&imu_topic_));
    auto flag = builder.SetLongName("--imu_topic")
                    .SetHelp("topic to publish imu")
                    .Build();
    imu_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&pose_topic_));
    auto flag = builder.SetLongName("--pose_topic")
                    .SetHelp("topic to publish pose")
                    .Build();
    pose_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    BoolFlag::Builder builder(MakeValueStore(&left_as_gray_scale_));
    auto flag = builder.SetLongName("--left_as_gray_scale")
                    .SetHelp("read left camera frame as a gray scale")
                    .Build();
    left_as_gray_scale_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    BoolFlag::Builder builder(MakeValueStore(&right_as_gray_scale_));
    auto flag = builder.SetLongName("--right_as_gray_scale")
                    .SetHelp("read right camera frame as a gray scale")
                    .Build();
    right_as_gray_scale_flag_ = std::make_unique<BoolFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&color_fps_, 30.f));
    auto flag = builder.SetLongName("--color_fps")
                    .SetHelp("color fps to load data (default: 30)")
                    .Build();
    color_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&depth_fps_, 30.f));
    auto flag = builder.SetLongName("--depth_fps")
                    .SetHelp("depth fps to load data (default: 30)")
                    .Build();
    depth_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&lidar_fps_, 5.5f));
    auto flag = builder.SetLongName("--lidar_fps")
                    .SetHelp("lidar fps to load data (default: 5.5)")
                    .Build();
    lidar_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&imu_fps_, 200.f));
    auto flag = builder.SetLongName("--imu_fps")
                    .SetHelp("imu fps to load data (default: 200)")
                    .Build();
    imu_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&pose_fps_, 200.f));
    auto flag = builder.SetLongName("--pose_fps")
                    .SetHelp("pose fps to load data (default: 200)")
                    .Build();
    pose_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
}

DatasetFlag::~DatasetFlag() = default;

int DatasetFlag::data_types() const {
  int ret = 0;
  if (left_color_topic_flag_->is_set()) {
    if (left_as_gray_scale_flag_->is_set()) {
      ret |= slam::SensorData::DATA_TYPE_LEFT_CAMERA_GRAY_SCALE;
    } else {
      ret |= slam::SensorData::DATA_TYPE_LEFT_CAMERA;
    }
  }
  if (right_color_topic_flag_->is_set()) {
    if (right_as_gray_scale_flag_->is_set()) {
      ret |= slam::SensorData::DATA_TYPE_RIGHT_CAMERA_GRAY_SCALE;
    } else {
      ret |= slam::SensorData::DATA_TYPE_RIGHT_CAMERA;
    }
  }
  if (depth_topic_flag_->is_set()) {
    ret |= slam::SensorData::DATA_TYPE_DEPTH_CAMERA;
  }
  if (lidar_topic_flag_->is_set()) {
    ret |= slam::SensorData::DATA_TYPE_LIDAR;
  }
  if (imu_topic_flag_->is_set()) {
    ret |= slam::SensorData::DATA_TYPE_IMU;
  }
  if (pose_topic_flag_->is_set()) {
    ret |= slam::SensorData::DATA_TYPE_GROUND_TRUTH_POSE;
  }
  return ret;
}

bool DatasetFlag::Parse(FlagParser& parser) {
  PARSE_POSITIONAL_FLAG(parser, 2, dataset_kind_flag_, path_flag_);
  if (dataset_kind_ == kEuroc) {
    current_dataset_kind_ = DATASET_KIND_EUROC;
  } else if (dataset_kind_ == kKitti) {
    current_dataset_kind_ = DATASET_KIND_KITTI;
  } else if (dataset_kind_ == kTum) {
    current_dataset_kind_ = DATASET_KIND_TUM;
  }
  return PARSE_OPTIONAL_FLAG(
      parser, name_flag_, channel_type_flag_, left_color_topic_flag_,
      right_color_topic_flag_, depth_topic_flag_, lidar_topic_flag_,
      imu_topic_flag_, pose_topic_flag_, left_as_gray_scale_flag_,
      right_as_gray_scale_flag_, color_fps_flag_, depth_fps_flag_,
      lidar_fps_flag_, imu_fps_flag_, pose_fps_flag_);
}

bool DatasetFlag::Validate() const {
  return CheckIfFlagWasSet(path_flag_) &&
         CheckIfFlagPositive(color_fps_flag_) &&
         CheckIfFlagPositive(depth_fps_flag_) &&
         CheckIfFlagPositive(lidar_fps_flag_) &&
         CheckIfFlagPositive(imu_fps_flag_) &&
         CheckIfFlagPositive(pose_fps_flag_) &&
         CheckIfOneOfFlagWasSet(left_color_topic_flag_, right_color_topic_flag_,
                                depth_topic_flag_, lidar_topic_flag_,
                                imu_topic_flag_, pose_topic_flag_);
}

}  // namespace felicia