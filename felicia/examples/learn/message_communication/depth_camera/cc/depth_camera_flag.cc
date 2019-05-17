#include "felicia/examples/learn/message_communication/depth_camera/cc/depth_camera_flag.h"

namespace felicia {

DepthCameraFlag::DepthCameraFlag() {
  {
    StringDefaultFlag::Builder builder(
        MakeValueStore<std::string>(&color_topic_, "color"));
    auto flag = builder.SetLongName("--color_topic")
                    .SetHelp("topic to publish for color frame, default: color")
                    .Build();
    color_topic_flag_ = std::make_unique<StringDefaultFlag>(flag);
  }
  {
    StringDefaultFlag::Builder builder(
        MakeValueStore<std::string>(&depth_topic_, "depth"));
    auto flag = builder.SetLongName("--depth_topic")
                    .SetHelp("topic to publish for depth frame, default: depth")
                    .Build();
    depth_topic_flag_ = std::make_unique<StringDefaultFlag>(flag);
  }
  {
    BoolFlag::Builder builder(MakeValueStore(&synched_));
    auto flag = builder.SetLongName("--synched")
                    .SetHelp("whether to read sensor data synchronously")
                    .Build();
    synched_flag_ = std::make_unique<BoolFlag>(flag);
  }
}

DepthCameraFlag::~DepthCameraFlag() = default;

bool DepthCameraFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, name_flag_, device_list_flag_,
                             device_index_flag_, color_topic_flag_,
                             depth_topic_flag_, synched_flag_);
}

bool DepthCameraFlag::Validate() const {
  if (device_list_flag_->is_set()) {
    PrintDeviceListFlagHelp();
    return true;
  }

  return CheckIfFlagWasSet(device_index_flag_);
}

}  // namespace felicia