#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_DEPTH_CAMERA_FLAG_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_DEPTH_CAMERA_FLAG_H_

#include "felicia/examples/learn/message_communication/common/cc/camera_flag.h"

namespace felicia {

class DepthCameraFlag : public CameraFlag {
 public:
  DepthCameraFlag();
  ~DepthCameraFlag();

  const StringFlag* color_topic_flag() const { return color_topic_flag_.get(); }
  const StringFlag* depth_topic_flag() const { return depth_topic_flag_.get(); }
  const StringFlag* pointcloud_topic_flag() const {
    return pointcloud_topic_flag_.get();
  }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(name_flag_, device_list_flag_,
                                          device_index_flag_, color_topic_flag_,
                                          depth_topic_flag_,
                                          pointcloud_topic_flag_)

 protected:
  std::string color_topic_;
  std::string depth_topic_;
  std::string pointcloud_topic_;
  std::unique_ptr<StringFlag> color_topic_flag_;
  std::unique_ptr<StringFlag> depth_topic_flag_;
  std::unique_ptr<StringFlag> pointcloud_topic_flag_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_DEPTH_CAMERA_FLAG_H_