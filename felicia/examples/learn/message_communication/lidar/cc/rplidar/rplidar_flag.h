#ifndef FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_RPLIDAR_RPLIDAR_FLAG_H_
#define FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_RPLIDAR_RPLIDAR_FLAG_H_

#include "felicia/examples/learn/message_communication/lidar/cc/lidar_flag.h"

namespace felicia {

class RPLidarFlag : public LidarFlag {
 public:
  RPLidarFlag();
  ~RPLidarFlag();

  const StringFlag* scan_mode_flag() const { return scan_mode_flag_.get(); }

  bool Parse(FlagParser& parser) override;

  AUTO_DEFINE_USAGE_AND_HELP_TEXT_METHODS(is_publishing_node_flag_, name_flag_,
                                          topic_flag_, serial_port_flag_,
                                          baudrate_flag_, ip_flag_,
                                          tcp_port_flag_, scan_mode_flag_)

 protected:
  std::string scan_mode_;
  std::unique_ptr<StringFlag> scan_mode_flag_;
};

}  // namespace felicia

#endif  // FELICIA_EXAMPLES_LEARN_MESSAGE_COMMUNICATION_LIDAR_CC_RPLIDAR_RPLIDAR_FLAG_H_