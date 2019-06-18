#include "felicia/examples/learn/message_communication/stereo_camera/cc/stereo_camera_flag.h"

namespace felicia {

StereoCameraFlag::StereoCameraFlag() {
  {
    StringDefaultFlag::Builder builder(
        MakeValueStore<std::string>(&left_camera_topic_, "left"));
    auto flag =
        builder.SetLongName("--left_camera_topic")
            .SetHelp("topic to publish for left camera frame, default: left")
            .Build();
    left_camera_topic_flag_ = std::make_unique<StringDefaultFlag>(flag);
  }
  {
    StringDefaultFlag::Builder builder(
        MakeValueStore<std::string>(&right_camera_topic_, "right"));
    auto flag =
        builder.SetLongName("--right_camera_topic")
            .SetHelp("topic to publish for right camera frame, default: right")
            .Build();
    right_camera_topic_flag_ = std::make_unique<StringDefaultFlag>(flag);
  }
  {
    StringDefaultFlag::Builder builder(
        MakeValueStore<std::string>(&depth_camera_topic_, "depth"));
    auto flag =
        builder.SetLongName("--depth_camera_topic")
            .SetHelp("topic to publish for depth camera frame, default: depth")
            .Build();
    depth_camera_topic_flag_ = std::make_unique<StringDefaultFlag>(flag);
  }
}

StereoCameraFlag::~StereoCameraFlag() = default;

bool StereoCameraFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, name_flag_, device_list_flag_,
                             device_index_flag_, left_camera_topic_flag_,
                             right_camera_topic_flag_,
                             depth_camera_topic_flag_);
}

bool StereoCameraFlag::Validate() const {
  if (device_list_flag_->is_set()) {
    PrintDeviceListFlagHelp();
    return true;
  }

  return CheckIfFlagWasSet(device_index_flag_);
}

}  // namespace felicia