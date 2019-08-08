#include "felicia/examples/learn/message_communication/depth_camera/cc/depth_camera_flag.h"

namespace felicia {

DepthCameraFlag::DepthCameraFlag(int default_width, int default_height,
                                 int default_fps, int default_pixel_format)
    : CameraFlag(default_width, default_height, default_fps,
                 default_pixel_format) {
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
                             device_index_flag_, width_flag_, height_flag_,
                             fps_flag_, pixel_format_flag_, color_topic_flag_,
                             depth_topic_flag_, pointcloud_topic_flag_);
}

bool DepthCameraFlag::Validate() const {
  if (CheckIfDeviceListFlagWasSet()) return true;
  return CheckIfCameraFlagsValid(true);
}

}  // namespace felicia