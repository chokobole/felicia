#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_COMMON_CC_CAMERA_FLAG_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_COMMON_CC_CAMERA_FLAG_H_

#include "felicia/drivers/camera/camera_format_message.pb.h"
#include "felicia/examples/learn/message_communication/common/cc/node_create_flag.h"

namespace felicia {

class CameraFlag : public NodeCreateFlag {
 public:
  static constexpr int kDefaultWidth = 640;
  static constexpr int kDefaultHeight = 480;
  static constexpr int kDefaultFps = 30;
  static constexpr int kDefaultPixelFormat = PIXEL_FORMAT_BGR;

  CameraFlag(int default_width = kDefaultWidth,
             int default_height = kDefaultHeight, int default_fps = kDefaultFps,
             int default_pixel_format = kDefaultPixelFormat);
  ~CameraFlag();

  const BoolFlag* device_list_flag() const { return device_list_flag_.get(); }
  const Flag<size_t>* device_index_flag() const {
    return device_index_flag_.get();
  }
  const IntDefaultFlag* width_flag() const { return width_flag_.get(); }
  const IntDefaultFlag* height_flag() const { return height_flag_.get(); }
  const IntDefaultFlag* fps_flag() const { return fps_flag_.get(); }
  const StringChoicesFlag* pixel_format_flag() const {
    return pixel_format_flag_.get();
  }

  bool Parse(FlagParser& parser) override;

  bool Validate() const override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(is_publishing_node_flag_, name_flag_,
                                          topic_flag_, device_list_flag_,
                                          device_index_flag_, width_flag_,
                                          height_flag_, fps_flag_,
                                          pixel_format_flag_)

  bool CheckIfDeviceListFlagWasSet() const;
  bool CheckIfCameraFlagsValid(bool is_publishing_node) const;

 protected:
  bool device_list_;
  size_t device_index_;
  int width_;
  int height_;
  int fps_;
  std::string pixel_format_;
  std::unique_ptr<BoolFlag> device_list_flag_;
  std::unique_ptr<Flag<size_t>> device_index_flag_;
  std::unique_ptr<IntDefaultFlag> width_flag_;
  std::unique_ptr<IntDefaultFlag> height_flag_;
  std::unique_ptr<IntDefaultFlag> fps_flag_;
  std::unique_ptr<StringChoicesFlag> pixel_format_flag_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_COMMON_CC_CAMERA_FLAG_H_