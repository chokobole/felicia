#include "felicia/examples/learn/message_communication/stereo_camera/cc/stereo_camera_flag.h"

namespace felicia {

StereoCameraFlag::StereoCameraFlag(int default_width, int default_height,
                                   int default_fps, int default_pixel_format)
    : CameraFlag(default_width, default_height, default_fps,
                 default_pixel_format) {
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
  return PARSE_OPTIONAL_FLAG(
      parser, name_flag_, device_list_flag_, device_index_flag_, width_flag_,
      height_flag_, fps_flag_, pixel_format_flag_, left_camera_topic_flag_,
      right_camera_topic_flag_, depth_topic_flag_, pointcloud_topic_flag_);
}

bool StereoCameraFlag::Validate() const {
  if (CheckIfDeviceListFlagWasSet()) return true;
  return CheckIfCameraFlagsValid(true);
}

}  // namespace felicia