#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_FLAG_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_FLAG_H_

#include "felicia/examples/learn/message_communication/common/cc/node_create_flag.h"

namespace felicia {

class CameraFlag : public NodeCreateFlag {
 public:
  CameraFlag();
  ~CameraFlag();

  const std::string& device_id() const { return device_id_; }
  const std::string& display_name() const { return display_name_; }

  bool Parse(FlagParser& parser) override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(name_flag_, topic_flag_,
                                          channel_type_flag_, device_id_flag_,
                                          display_name_flag_)

 private:
  std::string device_id_;
  std::string display_name_;
  std::unique_ptr<StringDefaultFlag> device_id_flag_;
  std::unique_ptr<StringDefaultFlag> display_name_flag_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_CAMERA_CC_CAMERA_FLAG_H_