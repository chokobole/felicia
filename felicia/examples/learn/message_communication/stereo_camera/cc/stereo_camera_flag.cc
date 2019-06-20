#include "felicia/examples/learn/message_communication/stereo_camera/cc/stereo_camera_flag.h"

namespace felicia {

StereoCameraFlag::StereoCameraFlag() {
  {
    StringFlag::Builder builder(
        MakeValueStore<std::string>(&left_camera_topic_));
    auto flag = builder.SetLongName("--left_camera_topic")
                    .SetHelp("topic to publish for left camera frame")
                    .Build();
    left_camera_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(
        MakeValueStore<std::string>(&right_camera_topic_));
    auto flag = builder.SetLongName("--right_camera_topic")
                    .SetHelp("topic to publish for right camera frame")
                    .Build();
    right_camera_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore<std::string>(&depth_topic_));
    auto flag = builder.SetLongName("--depth_topic")
                    .SetShortName("-d")
                    .SetHelp("topic to publish for depth frame")
                    .Build();
    depth_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    StringFlag::Builder builder(
        MakeValueStore<std::string>(&pointcloud_topic_));
    auto flag = builder.SetLongName("--pointcloud_topic")
                    .SetShortName("-p")
                    .SetHelp("topic to publish for pointcloud frame")
                    .Build();
    pointcloud_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
}

StereoCameraFlag::~StereoCameraFlag() = default;

bool StereoCameraFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, name_flag_, device_list_flag_,
                             device_index_flag_, left_camera_topic_flag_,
                             right_camera_topic_flag_, depth_topic_flag_,
                             pointcloud_topic_flag_);
}

bool StereoCameraFlag::Validate() const {
  if (device_list_flag_->is_set()) {
    PrintDeviceListFlagHelp();
    return true;
  }

  return CheckIfFlagWasSet(device_index_flag_);
}

}  // namespace felicia