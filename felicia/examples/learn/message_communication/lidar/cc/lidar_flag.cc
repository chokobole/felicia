#include "felicia/examples/learn/message_communication/lidar/cc/lidar_flag.h"

#include <iostream>

#include "felicia/core/util/command_line_interface/text_style.h"

namespace felicia {

LidarFlag::LidarFlag() {
  {
    StringFlag::Builder builder(MakeValueStore(&serial_port_));
    auto flag = builder.SetLongName("--serial_port")
                    .SetHelp("serial port to connect")
                    .Build();
    serial_port_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    FloatFlag::Builder builder(MakeValueStore(&buadrate_));
    auto flag = builder.SetLongName("--baudrate").Build();
    baudrate_flag_ = std::make_unique<FloatFlag>(flag);
  }
  {
    StringFlag::Builder builder(MakeValueStore(&ip_));
    auto flag = builder.SetLongName("--ip").SetHelp("ip to connect").Build();
    ip_flag_ = std::make_unique<StringFlag>(flag);
  }
  {
    Flag<uint16_t>::Builder builder(MakeValueStore(&tcp_port_));
    auto flag =
        builder.SetLongName("--port").SetHelp("port to connect").Build();
    tcp_port_flag_ = std::make_unique<Flag<uint16_t>>(flag);
  }
}

LidarFlag::~LidarFlag() = default;

bool LidarFlag::Parse(FlagParser& parser) {
  return PARSE_OPTIONAL_FLAG(parser, is_publishing_node_flag_, name_flag_,
                             topic_flag_, channel_type_flag_, serial_port_flag_,
                             baudrate_flag_, ip_flag_, tcp_port_flag_);
}

bool LidarFlag::Validate() const {
  if (is_publishing_node_) {
    if (serial_port_flag_->is_set() && baudrate_flag_->is_set()) {
      if (ip_flag_->is_set() || tcp_port_flag_->is_set()) {
        std::cerr << kRedError << "ip or tcp_port is set" << std::endl;
        return false;
      }
      return true;
    }

    if (ip_flag_->is_set() && tcp_port_flag_->is_set()) {
      if (serial_port_flag_->is_set() || baudrate_flag_->is_set()) {
        std::cerr << kRedError << "serial_port or baudrate is set" << std::endl;
        return false;
      }
      return true;
    }

    std::cerr << kRedError
              << "(serial_port, baudrate) or (ip, tcp_port) should be set "
              << std::endl;
  }

  return NodeCreateFlag::Validate();
}

}  // namespace felicia