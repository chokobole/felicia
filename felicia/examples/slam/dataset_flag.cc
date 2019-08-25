#include "felicia/examples/slam/dataset_flag.h"

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/protobuf/channel.pb.h"

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
    IntFlag::Builder builder(MakeValueStore(&data_kind_));
    auto flag = builder.SetName("data_kind")
                    .SetHelp(
                        "data kind, e.g) RGB or STEREO, which is an integer "
                        "value for enum DataKind for each dataset")
                    .Build();
    data_kind_flag_ = std::make_unique<IntFlag>(flag);
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
                    .SetHelp("topic to publish right color of stereo")
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
    FloatDefaultFlag::Builder builder(MakeValueStore(&color_fps_, 30.f));
    auto flag = builder.SetLongName("--color_fps")
                    .SetHelp("color fps to run slam (default: 30)")
                    .Build();
    color_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&depth_fps_, 30.f));
    auto flag = builder.SetLongName("--depth_fps")
                    .SetHelp("depth fps to run slam (default: 30)")
                    .Build();
    depth_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
  {
    FloatDefaultFlag::Builder builder(MakeValueStore(&lidar_fps_, 5.5f));
    auto flag = builder.SetLongName("--lidar_fps")
                    .SetHelp("lidar fps to run slam (default: 5.5)")
                    .Build();
    lidar_fps_flag_ = std::make_unique<FloatDefaultFlag>(flag);
  }
}

DatasetFlag::~DatasetFlag() = default;

bool DatasetFlag::Parse(FlagParser& parser) {
  PARSE_POSITIONAL_FLAG(parser, 3, dataset_kind_flag_, path_flag_,
                        data_kind_flag_);
  return PARSE_OPTIONAL_FLAG(parser, name_flag_, channel_type_flag_,
                             left_color_topic_flag_, right_color_topic_flag_,
                             depth_topic_flag_, lidar_topic_flag_,
                             color_fps_flag_, depth_fps_flag_, lidar_fps_flag_);
}

bool DatasetFlag::Validate() const {
  return CheckIfFlagWasSet(path_flag_) && CheckIfFlagWasSet(data_kind_flag_) &&
         CheckIfFlagPositive(color_fps_flag_) &&
         CheckIfFlagPositive(depth_fps_flag_) &&
         CheckIfFlagPositive(lidar_fps_flag_) &&
         CheckIfOneOfFlagWasSet(left_color_topic_flag_, right_color_topic_flag_,
                                depth_topic_flag_, lidar_topic_flag_);
}

}  // namespace felicia