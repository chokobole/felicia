#include "felicia/examples/learn/message_communication/depth_camera/cc/depth_camera_flag.h"

namespace felicia {

DepthCameraFlag::DepthCameraFlag() {
  {
    StringFlag::Builder builder(MakeValueStore<std::string>(&color_topic_));
    auto flag = builder.SetLongName("--color_topic")
                    .SetShortName("-c")
                    .SetHelp("topic to publish for color frame")
                    .Build();
    color_topic_flag_ = std::make_unique<StringFlag>(flag);
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

DepthCameraFlag::~DepthCameraFlag() = default;

bool DepthCameraFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, name_flag_, device_list_flag_,
                             device_index_flag_, color_topic_flag_,
                             depth_topic_flag_, pointcloud_topic_flag_);
}

bool DepthCameraFlag::Validate() const {
  if (device_list_flag_->is_set()) {
    PrintDeviceListFlagHelp();
    return true;
  }

  return CheckIfFlagWasSet(device_index_flag_);
}

}  // namespace felicia