#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_DEPTH_CAMERA_FLAG_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_DEPTH_CAMERA_FLAG_H_

#include "felicia/examples/learn/message_communication/common/cc/camera_flag.h"

namespace felicia {

class DepthCameraFlag : public CameraFlag {
 public:
  DepthCameraFlag();
  ~DepthCameraFlag();

  const StringDefaultFlag* color_topic_flag() const {
    return color_topic_flag_.get();
  }
  const StringDefaultFlag* depth_topic_flag() const {
    return depth_topic_flag_.get();
  }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(name_flag_, device_list_flag_,
                                          device_index_flag_, color_topic_flag_,
                                          depth_topic_flag_)

 private:
  std::string color_topic_;
  std::string depth_topic_;
  std::unique_ptr<StringDefaultFlag> color_topic_flag_;
  std::unique_ptr<StringDefaultFlag> depth_topic_flag_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_DEPTH_CAMERA_CC_DEPTH_CAMERA_FLAG_H_