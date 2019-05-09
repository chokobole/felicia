#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_COMMON_CC_CAMERA_FLAG_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_COMMON_CC_CAMERA_FLAG_H_

#include "felicia/examples/learn/message_communication/common/cc/node_create_flag.h"

namespace felicia {

class CameraFlag : public NodeCreateFlag {
 public:
  CameraFlag();
  ~CameraFlag();

  const BoolFlag* device_list_flag() const { return device_list_flag_.get(); }
  const Flag<size_t>* device_index_flag() const {
    return device_index_flag_.get();
  }
  const DefaultFlag<size_t>* buffer_size_flag() const {
    return buffer_size_flag_.get();
  }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(is_publishing_node_flag_, name_flag_,
                                          topic_flag_, channel_type_flag_,
                                          device_list_flag_, device_index_flag_,
                                          buffer_size_flag_)

 private:
  bool device_list_;
  size_t device_index_;
  size_t buffer_size_;
  std::unique_ptr<BoolFlag> device_list_flag_;
  std::unique_ptr<Flag<size_t>> device_index_flag_;
  std::unique_ptr<DefaultFlag<size_t>> buffer_size_flag_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_COMMON_CC_CAMERA_FLAG_H_