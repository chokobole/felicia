#include "felicia/examples/learn/message_communication/depth_camera/cc/realsense/rs_camera_flag.h"

namespace felicia {

RsCameraFlag::RsCameraFlag() {
  {
    StringFlag::Builder builder(MakeValueStore<std::string>(&imu_topic_));
    auto flag = builder.SetLongName("--imu_topic")
                    .SetHelp("topic to publish for imu")
                    .Build();
    imu_topic_flag_ = std::make_unique<StringFlag>(flag);
  }
}

RsCameraFlag::~RsCameraFlag() = default;

bool RsCameraFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, name_flag_, device_list_flag_,
                             device_index_flag_, color_topic_flag_,
                             depth_topic_flag_, pointcloud_topic_flag_,
                             imu_topic_flag_);
}

}  // namespace felicia