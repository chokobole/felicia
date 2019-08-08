#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_REALSENSE_RS_CAMERA_FLAG_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_REALSENSE_RS_CAMERA_FLAG_H_

#include "felicia/examples/learn/message_communication/depth_camera/cc/depth_camera_flag.h"

namespace felicia {

class RsCameraFlag : public DepthCameraFlag {
 public:
  RsCameraFlag(int default_width = kDefaultWidth,
               int default_height = kDefaultHeight,
               int default_fps = kDefaultFps);
  ~RsCameraFlag();

  const StringFlag* imu_topic_flag() const { return imu_topic_flag_.get(); }

  bool Parse(FlagParser& parser) override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(
      name_flag_, device_list_flag_, device_index_flag_, width_flag_,
      height_flag_, fps_flag_, pixel_format_flag_, color_topic_flag_,
      depth_topic_flag_, pointcloud_topic_flag_, imu_topic_flag_)

 private:
  std::string imu_topic_;

  std::unique_ptr<StringFlag> imu_topic_flag_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_REALSENSE_RS_CAMERA_FLAG_H_